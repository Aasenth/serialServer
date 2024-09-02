#include "logmanager.h"
#include "ui_logmanager.h"
#include "workmode.h"
#include <QDateTime>
#include <QFile>
#include <QDebug>

LogManager* LogManager::log = nullptr;

LogManager::LogManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogManager)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(true);
    ui->operatorText->setReadOnly(true);

//    workMode *wmlog = new workMode;
//    connect(wmlog,SIGNAL(send(QString)),this,SLOT(recive(QString)));
}

LogManager::~LogManager()
{
    delete ui;
}

//网络信号槽函数
void LogManager::recive(QString msg)
{
    this->nowMsg = msg;
    this->nowTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->textEdit->setTextColor(QColor(89,211,101));
    ui->textEdit->append(this->nowTime);
    ui->textEdit->setTextColor(QColor(133,133,133));
    ui->textEdit->append(this->nowMsg);
    this->WriteFileLog();
}

//数据操作槽函数
void LogManager::reciveOperator(QString msg)
{
    this->nowOperatorMsg = msg;
    this->nowTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    ui->operatorText->setTextColor(QColor(89,211,101));
    ui->operatorText->append(this->nowTime);
    ui->operatorText->setTextColor(QColor(133,133,133));
    ui->operatorText->append(this->nowOperatorMsg);
    this->WriteOperatorLog();
}

//网络信息写文件
void LogManager::WriteFileLog()
{
    QFile file("networkLog.txt");

    if(file.open(QIODevice::Append))
    {
        QString cont(this->nowTime +"\n" + this->nowMsg + "\n");
        QByteArray content = cont.toUtf8();
        file.write(content);
        file.close();
    }
    else {
        qDebug() << "文件打开失败";
    }
}

//数据操作写文件
void LogManager::WriteOperatorLog()
{
    QFile file("operatorLog.txt");

    if(file.open(QIODevice::Append))
    {
        QString cont(this->nowTime +"\n" + this->nowOperatorMsg + "\n");
        QByteArray content = cont.toUtf8();
        file.write(content);
        file.close();
    }
    else {
        qDebug() << "文件打开失败";
    }
}


LogManager* LogManager::getLog()
{
    if (!log)
    {
        log = new LogManager();
    }
    return log;
}
