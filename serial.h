#ifndef SERIAL_H
#define SERIAL_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>
class Serial : public QObject
{
    Q_OBJECT
public:
    explicit Serial(QObject *parent = nullptr);
    Serial(int id,int port_no,int com_no,QString com_nm,int baudrate,int databit,int stopbit,QString checkbit,QString flowcontrol,int rx_time,int tx_time);
signals:
    void readOk(QByteArray buf);
    void send(QString msg);
public:
    QByteArray buf;
    //打开串口
    void openSerial();
    // 发送数据
    void WriteData(QByteArray *data);
    // 发送数据
    void closeData();
    // 属性的获取
    int getPortNo();
    int getComNo();
    QString getComNm();
    int getBaudrate();
    int getDataBit();
    int getStopBit();
    QString getFlowcontrol();
    int getRxTime();
    int getTxTime();

private:
    int id; // id
    int port_no ; //端口
    int com_no;  //串口号
    QString com_nm; //串口名(填写与串口服务器通讯的上位机的 IP 地址)
    int baudrate; //波特率(1200/2400/4800/9600/38400/115200)
    QSerialPort::DataBits databit;  // 数据位(8/7/6/5)
    QSerialPort::StopBits stopbit;  // 停止位(1/2/3)
    QSerialPort::Parity checkbit; //校验位(无/奇校验/偶检验)
    QString flowcontrol; // 流控制(无/预留)
    int rx_time;   // 接收空闲时间
    int tx_time;   //tome发送空闲时间

    QSerialPort ports;

public slots:

       char *read_data(); // 接收到的数据
};

#endif // SERIAL_H
