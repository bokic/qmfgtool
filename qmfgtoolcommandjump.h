#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>


class QMfgToolCommandJump : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandJump(QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandJump();
    bool exec(QMfgToolDevice &device) override;

private:
    quint32 m_address;
};

