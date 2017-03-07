#pragma once

#include <QDateTime>
#include <QString>
#include <QMap>

enum QMfgToolSettingsDeviceState {MX_BOOTSTRAP, MX_UPDATER, MX_DISCONNECTED = 10};
enum QMfgToolSettingsDeviceChipFamily {UNKNOWN, MX50, MX6Q, MX6D, MX6SL, MX6SX, MX7D, MX6UL, MX6ULL};
enum QMfgToolSettingsDeviceType {DEV_HID_MX6Q, DEV_HID_MX6D, DEV_HID_MX6SL, DEV_HID_MX6SX, DEV_HID_MX7D, DEV_HID_MX6UL, DEV_HID_MX6ULL, DEV_HID_MX28, DEV_MSC_UPDATER, DEV_UNKNOWN};

enum QMfgToolMemorySection
{
    MemSectionOTH = 0x00,
    MemSectionAPP = 0xAA,
    MemSectionCSF = 0xCC,
    MemSectionDCD = 0xEE
};

struct QMfgToolSettingsLogItem
{
    enum QMfgToolSettingsLogItemType {Information, Warning, Error};

    QMfgToolSettingsLogItem(): type(Information) {}
    QMfgToolSettingsLogItem(QMfgToolSettingsLogItemType type, QString text): type(type), time(QDateTime::currentDateTime()), text(text) {}

    QMfgToolSettingsLogItemType type;
    QDateTime time;
    QString text;
};

struct QMfgToolSettingsCfgIni
{
    QString chip;
    QString board;
    QString list;
    QMap<QString, QString> variable;
};

struct QMfgToolSettingsDevice
{
    QMfgToolSettingsDeviceState name = MX_DISCONNECTED;
    QMfgToolSettingsDeviceType dev = DEV_UNKNOWN;
    QMfgToolSettingsDeviceChipFamily chipFamily = UNKNOWN;
    quint16 vid;
    quint16 pid;
};

struct QMfgToolSettingsList
{
    QString state;
    QString type;
    QString body;
    QString file;
    QString ifdev;
    QString address;
    QString loadSection;
    QString setSection;
    QString hasFlashHeader;
    QString description;
};

#pragma pack (push, 1)
struct SDPCmd
{
    quint16 command;
    quint8 format;
    quint32 address;
    quint32 dataCount;
    quint32 data;
};
#pragma pack (pop)
