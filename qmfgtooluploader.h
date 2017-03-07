#pragma once

#include <QObject>
#include <QString>


class QMfgToolUploader: public QObject
{
    Q_OBJECT
public:
    QMfgToolUploader();

signals:
    void processingCommand(QString status);
    void error(QString description);

public slots:
    void start();
};
