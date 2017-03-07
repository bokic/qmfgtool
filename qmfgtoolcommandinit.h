#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>


class QMfgToolCommandInit : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandInit(const QString &file, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandInit();
    bool exec(QMfgToolDevice &device) override;

private:
    QString m_file;
};
