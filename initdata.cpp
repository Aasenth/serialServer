#include "initdata.h"
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include<QMap>
InitData::InitData(QObject *parent) : QObject(parent)
{

}

QMap<int,Serial*> InitData::serialMap;

void InitData::init_db()
{
    // 链接Mysql数据库
    QSqlDatabase  db = QSqlDatabase::addDatabase("QMYSQL");
    // 配置ip 端口 用户名 密码 数据库信息
    db.setHostName("127.0.0.1");
    db.setPort(3306);
    db.setUserName("root");
    db.setPassword("123456");
    db.setDatabaseName("serialserver");

    if (db.open())
    {
        qDebug() << "数据库链接成功";
    }else {

        qDebug() << "链接数据库失败" << db.lastError().text();
    }
}
void InitData:: querySerial()
{
    QSqlQuery query;
    QString sql = "select id,port_no,com_no,com_nm,baudrate,databit,stopbit, checkbit,flowcontrol,rx_time,tx_time from serials";

    query.exec(sql);
    while(query.next())
    {
        // 配置串口信息
        int id = query.value("id").toInt();
        int port_no =query.value("port_no").toInt();
        int com_no =query.value("com_no").toInt();
        QString com_nm = query.value("com_nm").toString();
        int baudrate =query.value("baudrate").toInt();
        int databit =query.value("databit").toInt();
        int stopbit= query.value("stopbit").toInt();
        QString checkbit = query.value("checkbit").toString();
        QString flowcontrol = query.value("flowcontrol").toString();
        int rx_time = query.value("rx_time").toInt();
        int tx_time = query.value("tx_time").toInt();
//        qDebug() << id << port_no << com_no << com_nm << baudrate << databit << stopbit << checkbit << flowcontrol << rx_time << tx_time;
        Serial *serial = new Serial(id,port_no,com_no,com_nm,baudrate,databit,stopbit,checkbit,flowcontrol,rx_time,tx_time);
        serialMap[port_no]=  serial;
        //qDebug() << serial;
    }
}


//void InitData::getSerial(QSqlQuery query)
//{

//}




