#include "qmfgtoolcommandfind.h"
#include <QObject>


QMfgToolCommandFind::QMfgToolCommandFind(int timeout, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
    , m_timeout(timeout)
{
}

QMfgToolCommandFind::~QMfgToolCommandFind()
{
}

bool QMfgToolCommandFind::exec(QMfgToolDevice &device)
{
    Q_UNUSED(device);

    // Empty

    return true;
}
