#include "serial.h"
#include<QDebug>
#include "logmanager.h"
Serial::Serial(QObject *parent) : QObject(parent)
{

}

Serial::Serial(int id,int port_no,int com_no,QString com_nm,int baudrate,int databit,int stopbit,QString checkbit,QString flowcontrol,int rx_time,int tx_time)
{
    //链接信号槽，写日志
    connect(this,SIGNAL(send(QString)),LogManager::getLog(),SLOT(recive(QString)));

    this->id= id;
    this->port_no= port_no;
    this->com_no = com_no;
    this->com_nm = com_nm;
    switch(databit)
    {
    case 8:
        this->databit = QSerialPort::Data8;
        break;
    case 7:
        this->databit = QSerialPort::Data7;
        break;
    case 6:
        this->databit = QSerialPort::Data6;
        break;
    case 5:
        this->databit = QSerialPort::Data5;
        break;
    default:
        this->databit = QSerialPort::Data8;
        break;
    }
    this->baudrate = baudrate;
    switch(stopbit)
    {
    case 1:
        this->stopbit = QSerialPort::OneStop;
        break;
    case 2:
        this->stopbit = QSerialPort::TwoStop;
        break;
    case 3:
        this->stopbit = QSerialPort::OneAndHalfStop;
        break;
    default:
        databit = QSerialPort::OneStop;
        break;
    }
    if(checkbit == "E" || checkbit == "e")
    {
        this->checkbit = QSerialPort::EvenParity;
    }
    else if (checkbit == "O" || checkbit == "o")
    {
        this->checkbit = QSerialPort::OddParity;
    }
    else
    {
        this->checkbit = QSerialPort::NoParity;
    }
    this->flowcontrol = flowcontrol;
    this->rx_time = rx_time;
    this->tx_time = tx_time;
}
// 接收到的数据
char* Serial::read_data()
{
    //buf.clear();
    buf = ports.readAll();
    if(buf.size() > 0)
    {
//        qDebug() << com_nm << buf;

        QString msg(QString("串口%1发送数据%2").arg(com_nm).arg(QString(buf)));
        emit this->send(msg);
        emit this->readOk(buf);
    }
    return buf.data();

}
//打开串口
void Serial::openSerial()
{
    ports.setPortName(com_nm);
    ports.setBaudRate(baudrate);
    ports.setDataBits(databit);
    ports.setStopBits(stopbit);
    ports.setParity(checkbit);
    ports.setFlowControl(QSerialPort::NoFlowControl);

    if(ports.open(QIODevice::ReadWrite))
    {
        qDebug() << com_nm << "打开";
        QString msg(QString("串口%1打开").arg(com_nm));
        emit this->send(msg);
    }
    else {
        qDebug() << com_nm << "关闭";
        QString msg(QString("串口%1关闭").arg(com_nm));
        emit this->send(msg);
    }
    connect(&ports,SIGNAL(readyRead()),this,SLOT(read_data()),Qt::DirectConnection);


}
//发送
void Serial::WriteData(QByteArray *data)
{
    if(data == NULL)
    {
        qDebug() << "没有数据";
        return;
    }

    ports.write(*data);
//    qDebug() << "发送"<<*data;
    QString msg(QString("串口%1收到并返回数据%2").arg(com_nm).arg(QString(*data)));
    emit this->send(msg);
}
// 发送数据
//void Serial::closeData()
//{
//    if(buf.size()==0)
//    {
//        qDebug() << "数据为空";
//    }
//    else {
//        ports.write(buf);
//        qDebug() << "发送成功"<<buf.size();
//    }

//}

int Serial::getPortNo()
{
    return port_no;
}

int Serial:: getComNo()
{
    return com_no;
}
QString  Serial::getComNm()
{
    return com_nm;
}
int Serial:: getBaudrate()
{
    return baudrate;
}
int  Serial::getDataBit()
{
    return databit;
}
int  Serial::getStopBit()
{
    return stopbit;
}

QString  Serial::getFlowcontrol()
{
    return flowcontrol;
}
int Serial:: getRxTime()
{
    return rx_time;
}
int  Serial::getTxTime()
{
    return tx_time;
}
