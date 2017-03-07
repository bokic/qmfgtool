#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>


class QMfgToolCommandFind : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandFind(int timeout, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandFind();
    bool exec(QMfgToolDevice &device) override;

private:
    int m_timeout = 0;
};

