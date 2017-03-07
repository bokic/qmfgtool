#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgtooldevice.h"
#include "qmfgutils.h"
#include <QString>
#include <QObject>
#include <QFile>


class QMfgToolCommandLoad : public QMfgToolCommand
{
    Q_OBJECT
public:
    explicit QMfgToolCommandLoad(const QString &file, quint32 address, const QString &loadSection, const QString &setSection, bool hasFlashHeader, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent = nullptr);
    virtual ~QMfgToolCommandLoad();
    bool prepare(const QString &chip) override;
    bool exec(QMfgToolDevice &device) override;

private:
    QString m_file;
    QFile m_fileHandle;
    quint32 m_address = 0;
    QMfgToolMemorySection m_loadSection = MemSectionOTH;
    QMfgToolMemorySection m_setSection = MemSectionOTH;
    bool m_hasFlashHeader = false;
};

