#include "qmfgtoolcommandjump.h"
#include <QObject>


QMfgToolCommandJump::QMfgToolCommandJump(QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
{
}

QMfgToolCommandJump::~QMfgToolCommandJump()
{
}

bool QMfgToolCommandJump::exec(QMfgToolDevice &device)
{
    const quint16 ROM_KERNEL_CMD_JUMP_ADDR = 0x0B0B;

    quint32 address = device.loadAddress();

    if (address == 0)
    {
        return false;
    }

    SDPCmd SDPCmd;
    SDPCmd.command = ROM_KERNEL_CMD_JUMP_ADDR;
    SDPCmd.dataCount = 0;
    SDPCmd.format = 0;
    SDPCmd.data = 0;
    SDPCmd.address = address;

    if(!sendCmd(device, SDPCmd))
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("Failed to send jump command to ROM!!!"));
        return false;

    }

    if(!getHABType(device))
    {
        //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_FATAL_ERROR, _T("Failed to get HAB type from ROM, ignoredro!!!"));
        return false;
    }

    //LogMsg(LOG_MODULE_MFGTOOL_LIB, LOG_LEVEL_NORMAL_MSG, _T("*********MxHidDevice[%p] Jump to Ramkernel successfully!**********"), this);

    return true;
}
