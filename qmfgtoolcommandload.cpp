#include "qmfgtoolcommandload.h"
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QDir>


QMfgToolCommandLoad::QMfgToolCommandLoad(const QString &file, quint32 address, const QString &loadSection, const QString &setSection, bool hasFlashHeader, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
    , m_file(file)
    , m_address(address)
    , m_loadSection(MemSectionOTH)
    , m_setSection(MemSectionOTH)
    , m_hasFlashHeader(hasFlashHeader)
{
    if (loadSection.compare(QStringLiteral("APP"), Qt::CaseInsensitive) == 0)
    {
        m_loadSection = MemSectionAPP;
    }
    else if (loadSection.compare(QStringLiteral("CSF"), Qt::CaseInsensitive) == 0)
    {
        m_loadSection = MemSectionCSF;
    }
    else if (loadSection.compare(QStringLiteral("DCD"), Qt::CaseInsensitive) == 0)
    {
        m_loadSection = MemSectionDCD;
    }
    else
    {
        m_loadSection = MemSectionOTH;
    }

    if (setSection.compare(QStringLiteral("APP"), Qt::CaseInsensitive) == 0)
    {
        m_setSection = MemSectionAPP;
    }
    else if (setSection.compare(QStringLiteral("CSF"), Qt::CaseInsensitive) == 0)
    {
        m_setSection = MemSectionCSF;
    }
    else if (setSection.compare(QStringLiteral("DCD"), Qt::CaseInsensitive) == 0)
    {
        m_setSection = MemSectionDCD;
    }
    else
    {
        m_setSection = MemSectionOTH;
    }
}

QMfgToolCommandLoad::~QMfgToolCommandLoad()
{
}

bool QMfgToolCommandLoad::prepare(const QString &chip)
{
    QString filename;

    filename = QCoreApplication::applicationDirPath() + QDir::separator() + QStringLiteral("Profiles") + QDir::separator() + chip + QDir::separator() + QStringLiteral("OS Firmware") + QDir::separator() + m_file;

    m_fileHandle.setFileName(filename);

    return m_fileHandle.open(QIODevice::ReadOnly);
}

bool QMfgToolCommandLoad::exec(QMfgToolDevice &device)
{
    const quint32 MAX_SIZE_PER_DOWNLOAD_COMMAND = 0x200000; // 2MB

    quint32 byteIndex;
    quint32 numBytesToWrite = 0;
    quint32 dwFileSize = static_cast<quint32>(m_fileHandle.size());

    for ( byteIndex = 0; byteIndex < dwFileSize; byteIndex += numBytesToWrite )
    {
        QByteArray buf;

        // Get some data
        buf = m_fileHandle.read(MAX_SIZE_PER_DOWNLOAD_COMMAND);

        numBytesToWrite = static_cast<quint32>(buf.length());

        if (!transferData(device, m_address + byteIndex, numBytesToWrite, buf.constData()))
        {
            //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("Download(): TransData(0x%X, 0x%X, 0x%X, 0x%X) failed."), pImageParameter->PhyRAMAddr4KRL + byteIndex, numBytesToWrite, pImageParameter->loadSection, pBuffer + byteIndex);
            return false;
        }
    }

    // If we are downloading to DCD or CSF, we don't need to send
    if ((m_loadSection == MemSectionDCD)||(m_loadSection == MemSectionCSF))
    {
        return true;
    }

    if(m_setSection == MemSectionAPP)
    {
        /*quint32 ImageStartAddr = 0;

        if(device.deviceChipFamily() == MX50)
        {
            ImageStartAddr = m_address + m_codeOffset;
        }

        if(!AddIvtHdr(ImageStartAddr))
        {
            //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("DownloadImage(): AddHdr(0x%x) failed."),ImageStartAddr);
            return false;
        }*/

        qDebug() << "Unimplemented AddIvtHdr"; // TODO: Implemented AddIvtHdr.

        return false;
    }

    return true;
}
