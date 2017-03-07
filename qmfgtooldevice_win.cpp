#include "qmfgtooldevice.h"
#include <hidapi.h>


QMfgToolDevice::QMfgToolDevice(QObject *parent) : QObject(parent)
{
}

QMfgToolDevice::~QMfgToolDevice()
{
}


bool QMfgToolDevice::open()
{
    close();

    for(int c = 0; c < m_devices.length(); c++)
    {
        const QMfgToolSettingsDevice &device = m_devices.at(c);

        if (device.name != m_commandMode)
        {
            continue;
        }

        m_handle = hid_open(device.vid, device.pid, nullptr);

        if (m_handle)
        {
            m_deviceState = device.name;
            m_deviceType = device.dev;
            m_deviceChipFamily = device.chipFamily;

            return true;
        }
    }

    return false;
}

void QMfgToolDevice::close()
{
    if (m_handle)
    {
        hid_close(m_handle);
        m_handle = nullptr;
    }
}

bool QMfgToolDevice::isOpen() const
{
    return (m_handle != nullptr);
}

QByteArray QMfgToolDevice::read(int count)
{
    QByteArray ret;
    int readed = 0;

    if (!m_handle)
    {
        return ret;
    }

    ret.resize(count);
    ret.fill('\0');

    readed = hid_read_timeout(m_handle, reinterpret_cast<unsigned char *>(ret.data()), count, m_recvTimeoutMs);

    ret.resize(readed);

    return ret;
}

bool QMfgToolDevice::write(const QByteArray &data)
{
    int written = 0;

    if (!m_handle)
    {
        return false;
    }

    written = hid_write(m_handle, reinterpret_cast<const unsigned char *>(data.constData()), data.length());

    return written >= data.length();
}
