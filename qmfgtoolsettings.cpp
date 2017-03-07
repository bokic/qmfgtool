#include "qmfgtoolsettings.h"
#include "qmfgtoolcommandfind.h"
#include "qmfgtoolcommandboot.h"
#include "qmfgtoolcommandinit.h"
#include "qmfgtoolcommandload.h"
#include "qmfgtoolcommandjump.h"
#include "qmfgtoolcommandpush.h"
#include "qmfgtoolcommand.h"
#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QSettings>
#include <QDomNode>
#include <QThread>
#include <QRegExp>
#include <QFile>
#include <QList>
#include <QDir>


QMfgToolSettings::QMfgToolSettings()
{

}

QMfgToolSettings::~QMfgToolSettings()
{
    clearCommands();
}

bool QMfgToolSettings::load()
{
    QList<QMfgToolCommand *> bootstrapList, updaterList;
    m_cfgIni = QMfgToolSettingsCfgIni();
    m_devices.clear();
    m_log.clear();

    clearCommands();

    // Load ini
    QSettings settings(QCoreApplication::applicationDirPath() + QDir::separator() + QStringLiteral("cfg.ini"), QSettings::IniFormat);

    m_cfgIni = QMfgToolSettingsCfgIni();

    settings.beginGroup(QStringLiteral("profiles"));

    m_cfgIni.chip = settings.value(QStringLiteral("chip")).toString();
    if (m_cfgIni.chip.isEmpty())
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("Ini file missing data. '[profile] -> chip' is missing.")));
        return false;
    }

    settings.endGroup();


    settings.beginGroup(QStringLiteral("platform"));

    m_cfgIni.board = settings.value(QStringLiteral("board")).toString();
    if (m_cfgIni.board.isEmpty())
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("Ini file missing data. '[platform] -> board' is missing.")));
        return false;
    }

    settings.endGroup();

    settings.beginGroup(QStringLiteral("LIST"));

    m_cfgIni.list = settings.value(QStringLiteral("name")).toString();
    if (m_cfgIni.list.isEmpty())
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("Ini file missing data. '[name] -> board' is missing.")));
        return false;
    }

    settings.endGroup();

    settings.beginGroup(QStringLiteral("variable"));

    foreach(const QString &key, settings.allKeys())
    {
        m_cfgIni.variable.insert(key, settings.value(key).toString());
    }

    settings.endGroup();

    // Load xml
    QDomDocument domDocument;
    QString xmlFilePathString;
    QFile xmlFilepath;

    m_devices.clear();
    clearCommands();

    xmlFilePathString = QCoreApplication::applicationDirPath() + QDir::separator() + QStringLiteral("Profiles") + QDir::separator() + m_cfgIni.chip + QDir::separator() + QStringLiteral("OS Firmware") + QDir::separator() + QStringLiteral("ucl2.xml");
    xmlFilepath.setFileName(xmlFilePathString);

    if (!xmlFilepath.open(QIODevice::ReadOnly))
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("Can\'t open xml file. file: %1.").arg(xmlFilePathString)));
        return false;
    }

    if (!domDocument.setContent(&xmlFilepath))
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("Error parsing xml file. file: %1.").arg(xmlFilePathString)));
        return false;
    }

    QDomElement ucl = domDocument.documentElement();

    QDomElement cfg = ucl.firstChildElement(QStringLiteral("CFG"));

    for(int i = 0; i < cfg.childNodes().size(); i++)
    {
        QDomNode child = cfg.childNodes().at(i);

        if (child.isElement())
        {
            QDomElement childElement = child.toElement();

            if (childElement.nodeName() == QStringLiteral("STATE"))
            {
                QMfgToolSettingsDevice dev;
                QString nameStr, devStr;

                nameStr = childElement.attribute(QStringLiteral("name")).toLower();
                devStr = childElement.attribute(QStringLiteral("dev")).toUpper();

                if (nameStr == QStringLiteral("bootstrap"))
                {
                    dev.name = MX_BOOTSTRAP;
                }
                else if (nameStr == QStringLiteral("updater"))
                {
                    dev.name = MX_UPDATER;
                }
                else
                {
                    return false;
                }

                dev.dev = QMfgToolDevice::deviceType(devStr);
                dev.chipFamily = QMfgToolDevice::deviceChipFamily(devStr);

                if ((dev.dev == DEV_UNKNOWN)||(dev.chipFamily == UNKNOWN))
                {
                    return false;
                }

                // TODO: Add regex to validate pid/vid values.

                dev.vid = static_cast<quint16>(childElement.attribute(QStringLiteral("vid")).toInt(nullptr, 16));
                dev.pid = static_cast<quint16>(childElement.attribute(QStringLiteral("pid")).toInt(nullptr, 16));

                m_devices.append(dev);
            }
        }
    }

    if (m_devices.isEmpty())
    {
        m_log.append(QMfgToolSettingsLogItem(QMfgToolSettingsLogItem::Error, QObject::tr("No CFG xml tags found.")));
        return false;
    }

    for(int i = 0; i < ucl.childNodes().size(); i++)
    {
        QDomNode child = ucl.childNodes().at(i);
        if (child.isElement())
        {
            QDomElement listElement = child.toElement();

            if (listElement.nodeName() == QStringLiteral("LIST"))
            {
                QString listName = listElement.attribute(QStringLiteral("name"));

                if (listName == m_cfgIni.list)
                {
                    for(int j = 0; j < listElement.childNodes().size(); j++)
                    {
                        QDomNode child = listElement.childNodes().at(j);

                        if (child.isElement())
                        {
                            QDomElement cmdElement = child.toElement();

                            if (cmdElement.nodeName() == QStringLiteral("CMD"))
                            {
                                QDomNamedNodeMap attributes = cmdElement.attributes();

                                QString stateStr = attributes.namedItem(QStringLiteral("state")).nodeValue().trimmed().toLower();
                                QMfgToolSettingsDeviceState state = MX_DISCONNECTED;

                                QString type = attributes.namedItem(QStringLiteral("type")).nodeValue().trimmed().toLower();
                                QString body = attributes.namedItem(QStringLiteral("body")).nodeValue().trimmed().toLower();
                                QString ifdev = attributes.namedItem(QStringLiteral("ifdev")).nodeValue().trimmed().toUpper();
                                QString description = cmdElement.text().trimmed();

                                body = replaceKeywords(body);

                                QMfgToolCommand *command = nullptr;

                                if (stateStr == QStringLiteral("bootstrap"))
                                {
                                    state = MX_BOOTSTRAP;
                                }
                                else if (stateStr == QStringLiteral("updater"))
                                {
                                    state = MX_UPDATER;
                                }
                                else
                                {
                                    return false;
                                }

                                if (type == QStringLiteral("find"))
                                {
                                    int timeout = attributes.namedItem(QStringLiteral("timeout")).nodeValue().toInt();
                                    command = new QMfgToolCommandFind(timeout, state, body, description, ifdev);
                                }
                                else if (type == QStringLiteral("boot"))
                                {
                                    QString file = attributes.namedItem(QStringLiteral("file")).nodeValue();

                                    file = replaceKeywords(file);

                                    command = new QMfgToolCommandBoot(file, state, body, description, ifdev);
                                }
                                else if (type == QStringLiteral("init"))
                                {
                                    QString file = attributes.namedItem(QStringLiteral("file")).nodeValue();

                                    file = replaceKeywords(file);

                                    command = new QMfgToolCommandInit(file, state, body, description, ifdev);
                                }
                                else if (type == QStringLiteral("load"))
                                {
                                    QString file = attributes.namedItem(QStringLiteral("file")).nodeValue();
                                    quint32 address = attributes.namedItem(QStringLiteral("address")).nodeValue().toUInt(nullptr, 16);
                                    QString loadSection = attributes.namedItem(QStringLiteral("loadSection")).nodeValue();
                                    QString setSection = attributes.namedItem(QStringLiteral("setSection")).nodeValue();
                                    bool HasFlashHeader = attributes.namedItem(QStringLiteral("HasFlashHeader")).nodeValue().toLower() == QStringLiteral("true");

                                    file = replaceKeywords(file);
                                    loadSection = replaceKeywords(loadSection);
                                    setSection = replaceKeywords(setSection);

                                    command = new QMfgToolCommandLoad(file, address, loadSection, setSection, HasFlashHeader, state, body, description, ifdev);
                                }
                                else if (type == "jump")
                                {
                                    command = new QMfgToolCommandJump(state, body, description, ifdev);
                                }
                                else if (type == QStringLiteral("push"))
                                {
                                    QString file = attributes.namedItem(QStringLiteral("file")).nodeValue();
                                    QString savedFile = attributes.namedItem(QStringLiteral("savedFile")).nodeValue();
                                    bool ignoreError = attributes.namedItem(QStringLiteral("onError")).nodeValue().toLower() == QStringLiteral("ignore");

                                    file = replaceKeywords(file);
                                    savedFile = replaceKeywords(savedFile);

                                    command = new QMfgToolCommandPush(file, savedFile, ignoreError, state, body, description, ifdev);
                                }
                                /*else if (type == QStringLiteral("burn"))
                                {

                                }*/
                                else
                                {
                                    return false;
                                }

                                if (state == MX_BOOTSTRAP)
                                {
                                    bootstrapList.append(command);
                                }
                                else if (state == MX_UPDATER)
                                {
                                    updaterList.append(command);
                                }
                                else
                                {
                                    delete command;

                                    return false;
                                }
                            }
                        }
                    }

                    m_commands << bootstrapList << updaterList;

                    return true;
                }
            }
        }
    }

    return false;
}

bool QMfgToolSettings::prepare()
{
    for(int c = 0; c < m_commands.length(); c++)
    {
        if (!m_commands[c]->prepare(m_cfgIni.chip))
        {
            return false;
        }
    }

    return true;
}

QMfgToolSettingsCfgIni QMfgToolSettings::cfgIni() const
{
    return m_cfgIni;
}

QList<QMfgToolSettingsDevice> QMfgToolSettings::devices() const
{
    return m_devices;
}

QList<QMfgToolCommand *> QMfgToolSettings::commands() const
{
    return m_commands;
}

QString QMfgToolSettings::replaceKeywords(const QString &text)
{
    QRegExp regex(QStringLiteral("\\%(.*)\\%"));
    QString ret;
    int pos = 0;

    ret = text;

    regex.setMinimal(true);

    while((pos = regex.indexIn(ret, pos)) >= 0)
    {
        QString key = regex.cap(1);
        QString newValue = m_cfgIni.variable.value(key);

        ret.replace(pos, key.length() + 2, newValue);

        pos += newValue.length();
    }

    return ret;
}

void QMfgToolSettings::clearCommands()
{
    while(!m_commands.isEmpty())
    {
        delete m_commands.takeLast();
    }
}
