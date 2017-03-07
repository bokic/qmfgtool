#include "qmfgtoolcommandinit.h"
#include <QObject>


QMfgToolCommandInit::QMfgToolCommandInit(const QString &file, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
    , m_file(file)
{
}

QMfgToolCommandInit::~QMfgToolCommandInit()
{
}

bool QMfgToolCommandInit::exec(QMfgToolDevice &device)
{
    Q_UNUSED(device);

    // Empty

    return true;
}
