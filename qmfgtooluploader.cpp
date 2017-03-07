#include "qmfgtooluploader.h"
#include "qmfgtoolsettings.h"
#include "qmfgtooldevice.h"
#include <QApplication>
#include <QDomDocument>
#include <QDomElement>
#include <QSettings>
#include <QDomNode>
#include <QThread>
#include <QRegExp>
#include <QDebug>
#include <QFile>


QMfgToolUploader::QMfgToolUploader()
{

}

void QMfgToolUploader::start()
{
    QMfgToolSettings settings;
    QMfgToolDevice device;
    int commandIdx = 0;

    if (!settings.load())
    {
        emit error(tr("Error parsing XML file."));

        return;
    }

    if (settings.commands().count() == 0)
    {
        emit error(tr("No commands found in XML file."));

        return;
    }

    if (!settings.prepare())
    {
        emit error(tr("Error preparing commands."));

        return;
    }

    emit processingCommand(tr("Idle"));

    device.setDevices(settings.devices());

    auto commands = settings.commands();

    while(!QThread::currentThread()->isInterruptionRequested()) {

        auto command = commands[commandIdx];

        if (!device.setCommandMode(command->state())) {

            emit error(tr("Error changing command mode to `%1`.").arg(command->state()));

            break;
        }

        qDebug() << "isOpen()";
        if (!device.isOpen())
        {
            qDebug() << "open()";
            if (!device.open())
            {
                qDebug() << "msleep(100)";
                QThread::msleep(100);

                continue;
            }

            qDebug() << "HID device opened!";
        }

        emit processingCommand(command->description());

        if ((command->ifdev() == UNKNOWN)||(command->ifdev() == device.deviceChipFamily()))
        {
            qDebug() << "Exec command begin. index" << commandIdx;

            if (!command->exec(device))
            {
                emit error(tr("Error processing command."));

                qDebug() << "Exec command end. index" << commandIdx;

                break;
            }

            qDebug() << "Exec command end. index" << commandIdx;
        }

        commandIdx = (commandIdx + 1) % settings.commands().length();

        if (commandIdx == 0)
        {
            device.close();

            emit processingCommand(tr("Idle"));
        }
    }

    qDebug() << "exit loop";
}
