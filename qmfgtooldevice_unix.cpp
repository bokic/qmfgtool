#include "qmfgtooldevice.h"
#include "qmfgtoolsettings.h"
#include <libusb.h>


#define HID_SET_REPORT                  0x09
#define HID_REPORT_TYPE_OUTPUT          0x02
#define BLTC_CMD_INQUIRY                0x01
#define BLTC_CMD_DOWNLOAD_FIRMWARE      0x02
#define BLTC_PAGE_INQUIRY_CHIP_INFO     0x01


QMfgToolDevice::QMfgToolDevice(QObject *parent) : QObject(parent)
{
    libusb_init(&m_ctx);
}

QMfgToolDevice::~QMfgToolDevice()
{
    close();

    libusb_exit(m_ctx);
    m_ctx = nullptr;
}

bool QMfgToolDevice::open()
{
    int err = 0;

    close();

    for(int c = 0; c < m_devices.length(); c++)
    {
        const QMfgToolSettingsDevice &device = m_devices.at(c);

        if (device.name != m_commandMode)
        {
            continue;
        }

        m_handle = libusb_open_device_with_vid_pid(m_ctx, device.vid, device.pid);

        if (m_handle)
        {
            if (libusb_kernel_driver_active(m_handle, 0))
            {
                err = libusb_detach_kernel_driver(m_handle, 0);
                if ((err != 0)&&(err != LIBUSB_ERROR_NOT_FOUND))
                {
                    close();

                    continue;
                }
            }

            err = libusb_claim_interface(m_handle, 0);
            if (err)
            {
                close();

                continue;
            }

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
        libusb_close(m_handle);
        m_handle = nullptr;
    }

    m_deviceState = MX_DISCONNECTED;
    m_deviceType = DEV_UNKNOWN;
    m_deviceChipFamily = UNKNOWN;
}

bool QMfgToolDevice::isOpen() const
{
    return m_handle != nullptr;
}

QByteArray QMfgToolDevice::read(int count)
{
    QByteArray ret;
    int nBytesRead = -1;
    int err = 0;

    if (!m_handle)
    {
        return ret;
    }

    ret.resize(count);
    ret.fill('\0');

    err = libusb_interrupt_transfer(m_handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN, reinterpret_cast<unsigned char *>(ret.data()), count, &nBytesRead, m_recvTimeoutMs);

    if (err)
    {
        ret.clear();
    }
    else
    {
        ret.resize(nBytesRead);
    }

    return ret;
}

bool QMfgToolDevice::write(const QByteArray &data)
{
    int err = 0;

    if (!m_handle)
    {
        return false;
    }

    err = libusb_control_transfer(m_handle, LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE, HID_SET_REPORT, (HID_REPORT_TYPE_OUTPUT << 8) | data.at(0), 0, (unsigned char *)data.data(), static_cast<uint16_t>(data.length()), m_sendTimeoutMs);

    return err == data.count();
}
