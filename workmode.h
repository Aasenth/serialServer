#ifndef WORKMODE_H
#define WORKMODE_H

#include <QWidget>
#include<QSqlRelationalTableModel>
#include<QTcpServer>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include<QUdpSocket>

#include"serial.h"

namespace Ui {
class workMode;
}

class workMode : public QWidget
{
    Q_OBJECT

public:
    explicit workMode(QWidget *parent = nullptr);
    ~workMode();

signals:
    void send(QString msg);
    void sendOperator(QString msg);

private slots:
    void on_wmAddBtn_clicked();

    void on_wmSubBtn_clicked();

    void on_wmDelBtn_clicked();

    void on_wmReverseBtn_clicked();

    void on_wmSearchBtn_clicked();

    void on_wmSortCombox_currentIndexChanged(int index);

    void on_upSort_clicked();

    void on_downSort_clicked();

    void reciveOk(QByteArray buf,QTcpSocket *tcpSocket);

    void udpReciveOk(QByteArray buf,QUdpSocket *udpSocket,QHostAddress host,quint16 port);



private:
    Ui::workMode *ui;
    QSqlRelationalTableModel *wmModel;
    //QTcpServer *server;
    //QTcpSocket *serverSocket;
    //QTcpSocket *clientSocket;
   // QUdpSocket *udpSocket;
};

#endif // WORKMODE_H
