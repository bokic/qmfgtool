#pragma once

#include "qmfgtoolcommand.h"
#include "qmfgutils.h"
#include <QDateTime>
#include <QString>
#include <QList>
#include <QMap>


class QMfgToolSettings
{
public:
    QMfgToolSettings();
    ~QMfgToolSettings();
    bool load();
    bool prepare();

    QMfgToolSettingsCfgIni cfgIni() const;
    QList<QMfgToolSettingsDevice> devices() const;
    QList<QMfgToolCommand *> commands() const;

private:
    QString replaceKeywords(const QString &text);
    void clearCommands();
    QMfgToolSettingsCfgIni m_cfgIni;
    QList<QMfgToolSettingsDevice> m_devices;
    QList<QMfgToolCommand *> m_commands;
    QList<QMfgToolSettingsLogItem> m_log;
};
