#include "qmfgtooldevice.h"
#include "qmfgtoolsettings.h"

void QMfgToolDevice::setDevices(const QList<QMfgToolSettingsDevice> &devices)
{
    close();

    m_devices = devices;
}

QList<QMfgToolSettingsDevice> QMfgToolDevice::devices() const
{
    return m_devices;
}

void QMfgToolDevice::setLoadAddress(quint32 address)
{
    m_loadAddress = address;
}

quint32 QMfgToolDevice::loadAddress() const
{
    return m_loadAddress;
}


QMfgToolSettingsDeviceType QMfgToolDevice::deviceType() const
{
    return m_deviceType;
}

QMfgToolSettingsDeviceChipFamily QMfgToolDevice::deviceChipFamily() const
{
    return m_deviceChipFamily;
}

bool QMfgToolDevice::setCommandMode(QMfgToolSettingsDeviceState commandMode)
{
    if ((commandMode != MX_BOOTSTRAP)&&(commandMode != MX_UPDATER))
    {
        return false;
    }

    if (m_commandMode != commandMode)
    {
        close();

        m_commandMode = commandMode;
    }

    return true;
}

QMfgToolSettingsDeviceState QMfgToolDevice::commandMode() const
{
    return m_commandMode;
}

bool QMfgToolDevice::processCommand(const QMfgToolSettingsList &command)
{
    if (m_commandMode == MX_BOOTSTRAP)
    {
        if (command.type.compare("boot", Qt::CaseInsensitive) == 0)
        {

        }
        else if (command.type.compare("load", Qt::CaseInsensitive) == 0)
        {

        }
        else if (command.type.compare("jump", Qt::CaseInsensitive) == 0)
        {

        }
        else
        {
            return false;
        }
    }
    else if (m_commandMode == MX_UPDATER)
    {

    }
    else
    {
        return false;
    }

    return true;
}

QMfgToolSettingsDeviceType QMfgToolDevice::deviceType(const QString &name)
{
    QString nameVal;

    nameVal = name.trimmed().toUpper();

    if (nameVal == QStringLiteral("MX6Q"))
    {
        return DEV_HID_MX6Q;
    }
    else if (nameVal == QStringLiteral("MX6D"))
    {
        return DEV_HID_MX6D;
    }
    else if (nameVal == QStringLiteral("MX6SL"))
    {
        return DEV_HID_MX6SL;
    }
    else if (nameVal == QStringLiteral("MX6SX"))
    {
        return DEV_HID_MX6SX;
    }
    else if (nameVal == QStringLiteral("MX7D"))
    {
        return DEV_HID_MX7D;
    }
    else if (nameVal == QStringLiteral("MX28"))
    {
        return DEV_HID_MX28;
    }
    else if (nameVal == QStringLiteral("MX6UL"))
    {
        return DEV_HID_MX6UL;
    }
    else if (nameVal == QStringLiteral("MX6ULL"))
    {
        return DEV_HID_MX6ULL;
    }
    else if (nameVal == QStringLiteral("MSC"))
    {
        return DEV_MSC_UPDATER;
    }

    return DEV_UNKNOWN;
}

QMfgToolSettingsDeviceChipFamily QMfgToolDevice::deviceChipFamily(const QString &name)
{
    QString nameVal;

    nameVal = name.trimmed().toUpper();

    if (nameVal == QStringLiteral("MX6Q"))
    {
        return MX6Q;
    }
    else if (nameVal == QStringLiteral("MX6D"))
    {
        return MX6D;
    }
    else if (nameVal == QStringLiteral("MX6SL"))
    {
        return MX6SL;
    }
    else if (nameVal == QStringLiteral("MX6SX"))
    {
        return MX6SX;
    }
    else if (nameVal == QStringLiteral("MX7D"))
    {
        return MX7D;
    }
    else if (nameVal == QStringLiteral("MX6UL"))
    {
        return MX6UL;
    }
    else if (nameVal == QStringLiteral("MX6ULL"))
    {
        return MX6ULL;
    }

    return MX50; // WARNING: Possible incorrect value.
}
