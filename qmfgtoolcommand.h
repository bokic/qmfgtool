#pragma once

#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>

enum QMfgToolCommandLog: int {QError, QWarning, QInformation};

class QMfgToolCommand : public QObject
{
    Q_OBJECT
public:
    explicit QMfgToolCommand(QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommand();
    virtual bool prepare(const QString &chip);
    virtual bool exec(QMfgToolDevice &device) = 0;
    QMfgToolSettingsDeviceState state() const;
    QString body() const;
    QString description() const;
    QMfgToolSettingsDeviceChipFamily ifdev() const;

signals:
    void progress(int current, int total);
    void log(int type, QString text);

protected:
    QMfgToolSettingsDeviceChipFamily updateIfDev(const QString ifDev);

    bool sendCmd(QMfgToolDevice &device, const SDPCmd &cmd) const;
    bool sendData(QMfgToolDevice &device, const char data[], qint32 count) const;
    bool getCmdAck(QMfgToolDevice &device, quint32 value) const;    

    QByteArray packSDPCmd(const SDPCmd &cmd) const;
    bool getHABType(QMfgToolDevice &device) const;
    bool getDevAck(QMfgToolDevice &device, quint32 value) const;

    bool transferData(QMfgToolDevice &device, quint32 phyRam, quint32 count, const char data[]);

    QMfgToolSettingsDeviceChipFamily m_ifdev = UNKNOWN;
    QMfgToolSettingsDeviceState m_state = MX_DISCONNECTED;
    QString m_body;
    QString m_description;
};
