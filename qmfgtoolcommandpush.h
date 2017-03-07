#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>


class QMfgToolCommandPush : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandPush(const QString &file, const QString &savedFile, bool ignoreError, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandPush();
    bool exec(QMfgToolDevice &device) override;

private:
    QString m_file;
    QString m_savedFile;
    bool m_ignoreError = false;
};

