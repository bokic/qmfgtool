#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QByteArray>
#include <QString>
#include <QObject>
#include <QFile>


class QMfgToolCommandBoot : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandBoot(const QString &file, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandBoot();

    bool prepare(const QString &chip) override;
    bool exec(QMfgToolDevice &device) override;

private:
    QString m_file;
    QFile m_fileHandle;
    QByteArray m_data;
    void *m_fileContent = nullptr;

    bool DCDWrite(QMfgToolDevice &device, const char data[], quint32 count);
};

