#include "maindialog.h"
#include "ui_maindialog.h"
#include <QCloseEvent>
#include <QThread>
#include <QDebug>


MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    m_uploaderObject = new QMfgToolUploader();

    m_uploaderObject->moveToThread(&m_uploaderThread);

    connect(&m_uploaderThread, &QThread::started, this, &MainDialog::thread_started);
    connect(&m_uploaderThread, &QThread::finished, this, &MainDialog::thread_finished);
    connect(this, &MainDialog::startUploader, m_uploaderObject, &QMfgToolUploader::start);
    connect(m_uploaderObject, &QMfgToolUploader::processingCommand, this, &MainDialog::thread_processingCommand);

}

MainDialog::~MainDialog()
{
    m_uploaderThread.exit();
    m_uploaderThread.wait();

    delete m_uploaderObject;
    m_uploaderObject = nullptr;

    delete ui;
    ui = nullptr;
}

void MainDialog::closeEvent(QCloseEvent *event)
{
    if (ui->exit_pushButton->isEnabled())
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

void MainDialog::thread_started()
{
    qDebug() << "thread_started";

    ui->start_pushButton->setText(tr("&Stop"));
    ui->start_pushButton->setEnabled(true);
}

void MainDialog::thread_finished()
{
    qDebug() << "thread_finished";

    ui->start_pushButton->setText(tr("&Start"));
    ui->start_pushButton->setEnabled(true);
    ui->exit_pushButton->setEnabled(true);
}

void MainDialog::thread_processingCommand(QString status)
{
    ui->status_lineEdit->setText(status);
}

void MainDialog::on_start_pushButton_clicked()
{
    if (ui->start_pushButton->text() == tr("&Start"))
    {
        ui->start_pushButton->setEnabled(false);
        ui->exit_pushButton->setEnabled(false);

        m_uploaderThread.start();

        emit startUploader();
    }
    else if (ui->start_pushButton->text() == tr("&Stop"))
    {
        m_uploaderThread.requestInterruption();
        m_uploaderThread.exit();
    }
}
