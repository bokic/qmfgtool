#pragma once

#include "qmfgutils.h"
#include <QObject>

#ifdef Q_OS_UNIX
#include <libusb.h>
#elif defined Q_OS_WIN
#include <hidapi.h>
#elif defined Q_OS_OSX
#error OSX is not supported yet.
#else
#error Unsupported OS.
#endif


struct QMfgToolSettingsDevice;
struct QMfgToolSettingsList;

class QMfgToolDevice : public QObject
{
    Q_OBJECT
public:
    explicit QMfgToolDevice(QObject *parent = nullptr);
    ~QMfgToolDevice();

    bool open();
    void close();
    bool isOpen() const;

    QByteArray read(int count);
    bool write(const QByteArray &data);

    void setDevices(const QList<QMfgToolSettingsDevice> &devices);
    QList<QMfgToolSettingsDevice> devices() const;

    void setLoadAddress(quint32 address);
    quint32 loadAddress() const;

    QMfgToolSettingsDeviceType deviceType() const;
    QMfgToolSettingsDeviceChipFamily deviceChipFamily() const;

    bool setCommandMode(QMfgToolSettingsDeviceState commandMode);
    QMfgToolSettingsDeviceState commandMode() const;

    bool processCommand(const QMfgToolSettingsList &command);

    static QMfgToolSettingsDeviceType deviceType(const QString &name);
    static QMfgToolSettingsDeviceChipFamily deviceChipFamily(const QString &name);

private:
    QList<QMfgToolSettingsDevice> m_devices;
    QMfgToolSettingsDeviceState m_commandMode = MX_DISCONNECTED;

    QMfgToolSettingsDeviceState m_deviceState;
    QMfgToolSettingsDeviceType m_deviceType;
    QMfgToolSettingsDeviceChipFamily m_deviceChipFamily;

    quint32 m_loadAddress = 0;

    unsigned int m_sendTimeoutMs = 10000;
    unsigned int m_recvTimeoutMs = 1000;

#ifdef Q_OS_UNIX
    libusb_context *m_ctx = nullptr;
    libusb_device_handle *m_handle = nullptr;
#elif defined Q_OS_WIN
    hid_device *m_handle = nullptr;
#elif defined Q_OS_OSX
#endif
};
