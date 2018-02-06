#include "qmfgtoolcommand.h"
#include <QString>

QMfgToolCommand::QMfgToolCommand(QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QObject (parent)
    , m_ifdev(UNKNOWN)
    , m_state(state)
    , m_body(body)
    , m_description(description)
{
    m_ifdev = updateIfDev(ifdev);
}

QMfgToolCommand::~QMfgToolCommand()
{
}

bool QMfgToolCommand::prepare(const QString &chip)
{
    Q_UNUSED(chip);

    return true;
}

QMfgToolSettingsDeviceState QMfgToolCommand::state() const
{
    return m_state;
}

QString QMfgToolCommand::body() const
{
    return m_body;
}

QString QMfgToolCommand::description() const
{
    return m_description;
}

QMfgToolSettingsDeviceChipFamily QMfgToolCommand::ifdev() const
{
    return m_ifdev;
}

QMfgToolSettingsDeviceChipFamily QMfgToolCommand::updateIfDev(const QString ifDev)
{
    if (ifDev.compare(QStringLiteral("MX50")) == 0)
    {
        return MX50;
    }
    else if (ifDev.compare(QStringLiteral("MX6Q")) == 0)
    {
        return MX6Q;
    }
    else if (ifDev.compare(QStringLiteral("MX6D")) == 0)
    {
        return MX6D;
    }
    else if (ifDev.compare(QStringLiteral("MX6SL")) == 0)
    {
        return MX6SL;
    }
    else if (ifDev.compare(QStringLiteral("MX6SX")) == 0)
    {
        return MX6SX;
    }
    else if (ifDev.compare(QStringLiteral("MX7D")) == 0)
    {
        return MX7D;
    }
    else if (ifDev.compare(QStringLiteral("MX6UL")) == 0)
    {
        return MX6UL;
    }
    else if (ifDev.compare(QStringLiteral("MX6ULL")) == 0)
    {
        return MX6ULL;
    }

    return UNKNOWN;
}

bool QMfgToolCommand::sendCmd(QMfgToolDevice &device, const SDPCmd &cmd) const
{
    QByteArray packet;

    //First, pack the command to a report.
    packet = packSDPCmd(cmd);

    //Send the report to USB HID device
    if (!device.write(packet))
    {
        return false;
    }

    return true;
}

bool QMfgToolCommand::sendData(QMfgToolDevice &device, const char data[], qint32 count) const
{
    const char REPORT_ID_DATA =	2;

    QByteArray tmp(1025, '\0');

    if (count > 1024)
    {
        return false;
    }

    tmp[0] = REPORT_ID_DATA;

    memcpy(tmp.data() + 1, data, static_cast<size_t>(count));

    if (!device.write(tmp))
    {
        return false;
    }

    return true;
}

bool QMfgToolCommand::getCmdAck(QMfgToolDevice &device, quint32 value) const
{
    if(!getHABType(device))
    {
        return false;
    }

    if(!getDevAck(device, value))
    {
        return false;
    }

    return true;
}

QByteArray QMfgToolCommand::packSDPCmd(const SDPCmd &cmd) const
{
    const char REPORT_ID_SDP_CMD = 1;

    QByteArray ret(17, '\0');

    ret[0] = REPORT_ID_SDP_CMD;

    quint32 *pTmpSDPCmd = reinterpret_cast<quint32 *>(ret.data() + 1);

    pTmpSDPCmd[0] = (((cmd.address  & 0x00FF0000) << 8)
                  | ((cmd.address   & 0xFF000000) >> 8)
                  |  (cmd.command   & 0x0000FFFF) );

    pTmpSDPCmd[1] = ((cmd.dataCount & 0xFF000000)
                  | ((cmd.format    & 0x000000FF) << 16)
                  | ((cmd.address   & 0x000000FF) <<  8)
                  | ((cmd.address   & 0x0000FF00) >>  8 ));

    pTmpSDPCmd[2] = ((cmd.data      & 0xFF000000)
                  | ((cmd.dataCount & 0x000000FF) << 16)
                  |  (cmd.dataCount & 0x0000FF00)
                  | ((cmd.dataCount & 0x00FF0000) >> 16));

    pTmpSDPCmd[3] = (((0x00         & 0x000000FF) << 24)
                  | ((cmd.data     & 0x00FF0000) >> 16)
                  |  (cmd.data     & 0x0000FF00)
                  | ((cmd.data     & 0x000000FF) << 16));

    return ret;
}

bool QMfgToolCommand::getHABType(QMfgToolDevice &device) const
{
    const quint32 HabEnabled  = 0x12343412;
    const quint32 HabDisabled = 0x56787856;

    QByteArray tmp;

    //Get Report3, Device to Host:
    //4 bytes HAB mode indicating Production/Development part
    tmp = device.read(1025);
    if ( tmp.length() < 5)
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("Failed to read HAB type from ROM!!!"));
        return false;
    }
    if ( (*reinterpret_cast<const quint32 *>(tmp.constData() + 1) != HabEnabled)  &&
         (*reinterpret_cast<const quint32 *>(tmp.constData() + 1) != HabDisabled) )
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("HAB type mismatch: 0x%x!!!"), *(unsigned int *)(m_pReadReport->Payload));
        return false;
    }

    return true;
}

bool QMfgToolCommand::getDevAck(QMfgToolDevice &device, quint32 value) const
{
    QByteArray tmp;

    //Get Report4, Device to Host:
    tmp = device.read(1025);
    if ( tmp.length() < 5)
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("Failed to read ACK!!!"));
        return false;
    }

    if (*reinterpret_cast<const quint32 *>(tmp.constData() + 1) != value)
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("WriteReg(): Invalid write ack: 0x%x\n"), ((PULONG)m_pReadReport)[0]);
        return false;
    }

    return true;
}

bool QMfgToolCommand::transferData(QMfgToolDevice &device, quint32 phyRam, quint32 count, const char data[])
{
    const quint16 ROM_KERNEL_CMD_WR_FILE = 0x0404;
    const quint32 ROM_STATUS_ACK = 0x88888888;

    SDPCmd SDPCmd;

    SDPCmd.command = ROM_KERNEL_CMD_WR_FILE;
    SDPCmd.dataCount = count;
    SDPCmd.format = 0;
    SDPCmd.data = 0;
    SDPCmd.address = phyRam;

    //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_NORMAL_MSG, _T(" Trans Data address is %x \n"),address);
    if(!sendCmd(device, SDPCmd))
    {
        return false;
    }

    qint32 MaxHidTransSize = 1025 - 1;

    int chunks = (count % MaxHidTransSize)?((count / MaxHidTransSize) + 1): (count / MaxHidTransSize);

    for(int c = 0; c < chunks; c++)
    {
        qint32 chunkSize = (count > MaxHidTransSize) ? MaxHidTransSize : count;
        if(!sendData(device, data, chunkSize))
        {
            return false;
        }

        emit progress(c, chunks);

        count -= chunkSize;
        data += chunkSize;
    }

    //below function should be invoked for mx50
    if ( !getCmdAck(device, ROM_STATUS_ACK) )
    {
        return false;
    }

    return true;
}
