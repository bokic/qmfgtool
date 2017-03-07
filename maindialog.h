#pragma once

#include "qmfgtooluploader.h"
#include <QDialog>
#include <QThread>
#include <QEvent>


namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

signals:
    void startUploader();
    void stopUploader();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void thread_started();
    void thread_finished();
    void thread_processingCommand(QString status);
    void on_start_pushButton_clicked();

private:
    Ui::MainDialog *ui;
    QMfgToolUploader *m_uploaderObject = nullptr;
    QThread m_uploaderThread;
};
