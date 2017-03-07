#include "qmfgtoolcommandpush.h"
#include <QObject>


QMfgToolCommandPush::QMfgToolCommandPush(const QString &file, const QString &savedFile, bool ignoreError, QMfgToolSettingsDeviceState state, const QString &body, const QString &description, const QString &ifdev, QObject *parent)
    : QMfgToolCommand(state, body, description, ifdev, parent)
    , m_file(file)
    , m_savedFile(savedFile)
    , m_ignoreError(ignoreError)
{
}

QMfgToolCommandPush::~QMfgToolCommandPush()
{
}

bool QMfgToolCommandPush::exec(QMfgToolDevice &device)
{
    Q_UNUSED(device);

    // retValue = ((MFGLIB_VARS *)m_pLibVars)->g_CmdOperationArray[index]->m_pUTP->UtpWrite(csCmdBody, m_FileName, index);

    return false;
}
