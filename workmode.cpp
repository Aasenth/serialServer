#include "workmode.h"
#include "ui_workmode.h"
#include<QSqlDatabase>
#include<QSqlError>
#include<QDebug>
#include<QSqlTableModel>
#include<QSqlRecord>
#include<QItemSelectionModel>
#include<QModelIndexList>
#include<QSqlRelationalTableModel>
#include<QSqlRelationalDelegate>
#include<QSqlRelationalTableModel>
#include<QTcpServer>
#include<QTcpSocket>
#include<QSqlQuery>
#include<QSet>
#include<QMap>

#include<QTimer>
#include<serial.h>
#include"initdata.h"


#include "logmanager.h"

workMode::workMode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::workMode)
{
    ui->setupUi(this);

    //2.启动程序时，在表格中显示表的数据
    this->wmModel = new QSqlRelationalTableModel(this);

    //设置管理的数据表表名
    this->wmModel->setTable("wm");
    //查询表中所有数据
    this->wmModel->select();
    //将查询结果显示在tableview当中
    ui->tableView->setModel(this->wmModel);

    //设置关联表的下拉菜单
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));

    //注意：查询完成后才可以设置表头
    this->wmModel->setHeaderData(0,Qt::Horizontal,"工作模式");
    this->wmModel->setHeaderData(1,Qt::Horizontal,"tcp数据模式");
    this->wmModel->setHeaderData(2,Qt::Horizontal,"串口号");
    this->wmModel->setHeaderData(3,Qt::Horizontal,"换行符解释");
    this->wmModel->setHeaderData(4,Qt::Horizontal,"回车符解释");
    this->wmModel->setHeaderData(5,Qt::Horizontal,"最大连接数");
    this->wmModel->setHeaderData(6,Qt::Horizontal,"通信协议");
    this->wmModel->setHeaderData(7,Qt::Horizontal,"主机地址");
    this->wmModel->setHeaderData(8,Qt::Horizontal,"主机端口");
    this->wmModel->setHeaderData(9,Qt::Horizontal,"连接策略");
    this->wmModel->setHeaderData(10,Qt::Horizontal,"断开策略");
    this->wmModel->setHeaderData(11,Qt::Horizontal,"空闲时间");

    //设置隔行变色
    ui->tableView->setAlternatingRowColors(true);

    //设置表格宽度 stretch占据整个宽度 resizeTocontent 根据内容设置宽度
    //ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //设置表格的修改属性 onmanuaSumbit手动提交
    this->wmModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    //链接信号书写日志
    connect(this,SIGNAL(send(QString)),LogManager::getLog(),SLOT(recive(QString)));
    connect(this,SIGNAL(sendOperator(QString)),LogManager::getLog(),SLOT(reciveOperator(QString)));

    ////////////////////////// 2 连接，动态创建服务器,写入串口 /////////////////////////////////////
    //获取连接方法和主机地址与端口
    int rowCount = this->wmModel->rowCount();  // 获取表格中的总行数
    int protocolColumnIndex = this->wmModel->fieldIndex("protocol");  // 获取 protocol 列的索引
    int peerHostColumnIndex = this->wmModel->fieldIndex("peerhost");  // 获取 peerhost 列的索引
    int peerPortColumnIndex = this->wmModel->fieldIndex("peerport");  // 获取 peerport 列的索引
    int port_noIndex = this->wmModel->fieldIndex("port_no");

    for (int row = 0; row < rowCount; ++row)
      {
          QString protocol = this->wmModel->record(row).value(protocolColumnIndex).toString();

          /////////////////////////////////////tcp服务器模式///////////////////////////////////

          if (protocol == "1")
          {
              QString peerhost =this->wmModel->record(row).value(peerHostColumnIndex).toString();
              quint16 peerport = this->wmModel->record(row).value(peerPortColumnIndex).toUInt();
              int port_no =this->wmModel->record(row).value(port_noIndex).toInt();
//              qDebug()<<port_no;

              //根据串口编号获取串口对象并且初始化
              Serial *serial=new Serial;
              serial = InitData::serialMap[port_no];

              //qDebug()<<"45565"<<serial->getComNm();
              serial->openSerial();

              // 创建服务器
              QTcpServer *server = new QTcpServer(this);
              if (!server->listen(QHostAddress::Any, peerport)) {
                  qDebug() << "无法监听端口" << peerport << ":" << server->errorString();
                  QString msg(QString("tcp服务器模式----无法监听端口：%1").arg(peerport));
                  emit this->send(msg);
                  delete server;
                  continue;
              }

              qDebug() << "tcp服务器:成功创建服务器并监听端口：" << peerport;
              QString msg(QString("tcp服务器模式----成功创建服务器并监听端口：%1").arg(peerport));
              emit this->send(msg);

              // 当有客户端连接时，处理连接
              connect(server, &QTcpServer::newConnection, [=]() {
                  QTcpSocket *serverSocket = server->nextPendingConnection();
                  QString ip = serverSocket->peerAddress().toString();
                  quint16 port = serverSocket->peerPort();

                  qDebug() << "客户端连接：" << ip << port;
                  QString msg(QString("tcp服务器模式----ip为%1,port为%2的客户端连入").arg(ip).arg(port));
                  emit this->send(msg);
                  // 读取串口数据
                  connect(serial, &Serial::readOk, this, [=](QByteArray buf){
                      this->reciveOk(buf,serverSocket);
                      QString msg(QString("tcp服务器模式----接收到串口发送的数据%1返回给主机").arg(QString(buf)));
                      emit this->send(msg);
                  });
                  // 处理客户端数据
                  connect(serverSocket, &QTcpSocket::readyRead, [=]() {
                      QByteArray buf = serverSocket->readAll();
                      QString msg(QString("tcp服务器模式----收到客户端发来的数据:%1").arg(QString(buf)));
                      emit this->send(msg);
                      qDebug() << "收到数据：" << msg;
                      serial->WriteData(&buf);
                  });
              });       
          }

          ///////////////////tcp客户端模式///////////////////////////////////////////////////
          else if(protocol == '2')
          {
             QTcpSocket *clientSocket=new QTcpSocket(this);
             // 从库中获取主机地址和端口信息
             QString peerhost =this->wmModel->record(row).value(peerHostColumnIndex).toString();
             quint16 peerport = this->wmModel->record(row).value(peerPortColumnIndex).toUInt();
             int port_no =this->wmModel->record(row).value(port_noIndex).toInt();
             //qDebug()<<peerhost<<peerport;
             clientSocket->connectToHost(peerhost,peerport);

             // 定义空闲时间（毫秒）
             int idleTime = this->wmModel->record(row).value(this->wmModel->fieldIndex("freetime")).toInt() * 1000;

             // 创建一个定时器，用于检测空闲超时
             QTimer *freeTimer = new QTimer(this);
             freeTimer->setInterval(idleTime);

             //根据串口编号获取串口对象并且初始化
             Serial *serial=new Serial;
             serial = InitData::serialMap[port_no];
             serial->openSerial();

             connect(clientSocket,&QTcpSocket::connected,[=](){
                 qDebug()<<QString("tcp客户端模式----链接服务器成功,服务器ip:%1,port:%2").arg(peerhost).arg(peerport);
                 QString msg(QString("tcp客户端模式----链接服务器成功,服务器ip:%1,port:%2").arg(peerhost).arg(peerport));
                 emit this->send(msg);
                 freeTimer->start();

                 // 读取串口数据,将其返回
                 connect(serial, &Serial::readOk, this, [=](QByteArray buf){
                     this->reciveOk(buf,clientSocket);
                     QString msg(QString("tcp客户端模式----接收到串口发送的数据%1").arg(QString(buf)));
                     emit this->send(msg);
                 });

             });

             connect(clientSocket,&QTcpSocket::readyRead,[=](){
                 //从socket中读取客户端的信息
                 QByteArray buf = clientSocket->readAll();
                 QString msg(QString("tcp客户端模式----客户端收到数据:%1").arg(QString(buf)));
                 emit this->send(msg);
                 qDebug()<<msg;
                 freeTimer->start();
                 serial->WriteData(&buf);
             });

             // 当定时器超时时，断开连接
             connect(freeTimer, &QTimer::timeout, [=]() {
                 qDebug() << "tcp客户端模式----超过空闲时间，断开连接";
                 QString msg("tcp客户端模式----超过空闲时间，断开连接");
                 emit this->send(msg);

                 clientSocket->disconnectFromHost();
                 freeTimer->stop();  // 停止定时器
             });

          }
          ///////////////////udp模式////////////////////////////////////////////////////
          else if(protocol == '3')
          {
              // 1. 实例化socket对象
              QUdpSocket *udpSocket = new QUdpSocket(this);
              // 绑定端口和ip
              QString peerhost =this->wmModel->record(row).value(peerHostColumnIndex).toString();
              quint16 peerport = this->wmModel->record(row).value(peerPortColumnIndex).toUInt();
              int port_no =this->wmModel->record(row).value(port_noIndex).toInt();

              udpSocket->bind(QHostAddress(peerhost), peerport);
              qDebug()<<"udp连接"<<peerport;
              QString msg(QString("udp模式----udp链接host:%1 port:%2").arg(peerhost).arg(peerport));
              emit this->send(msg);

              //根据串口编号获取串口对象并且初始化
              Serial *serial=new Serial;
              serial = InitData::serialMap[port_no];
              serial->openSerial();

              // 3. 一旦有数据接收，就会触发 readyRead 信号
              connect(udpSocket, &QUdpSocket::readyRead, [=](){
                  char buf[1024] = {0};
                  QHostAddress host;
                  quint16 port;
                  udpSocket->readDatagram(buf, 1024, &host, &port);

                  qDebug() <<"收到来自" << host.toString() << port <<"的消息";
                  QString msg(QString("udp模式----收到来自ip:%1,port:%2发送的消息:%3").arg(host.toString()).arg(port).arg(QString(buf)));
                  emit this->send(msg);

                  // 将数据写入串口
                  QByteArray dataToWrite(buf);
                  serial->WriteData(&dataToWrite);
                  qDebug() << "udp连接：已将数据写入串口";

                  // 读取串口数据,将其返回
                  static bool isSerialConnected = false;  // 保证只连接一次
                  if (!isSerialConnected) {
                      connect(serial, &Serial::readOk, this, [=](QByteArray buf){
                          this->udpReciveOk(buf, udpSocket, host, port);
                          QString msg(QString("udp模式----接收到串口发送的数据%1返回给主机").arg(QString(buf)));
                          emit this->send(msg);
                      });
                      isSerialConnected = true;
                  }
              });
          }
    }
}

workMode::~workMode()
{
    delete ui;
}

//添加
void workMode::on_wmAddBtn_clicked()
{
    //1.创建一个空行 (此时，该空行和模型还没有关系，只是内存中创建出一个空行)
    QSqlRecord record = this->wmModel->record();

    //2获取模型的总行书
    int num=this->wmModel->rowCount();

    //将创建的空行，添加到model的最后一行
    this->wmModel->insertRecord(num,record);
    QString msg("工作模式模块-----添加了信息");
    emit this->sendOperator(msg);
}

//提交
void workMode::on_wmSubBtn_clicked()
{
    this->wmModel->submitAll();

}

//删除
void workMode::on_wmDelBtn_clicked()
{
    //1.获取所有选中的行对象
    QItemSelectionModel *selectionModel=ui->tableView->selectionModel();
    //2.从选中的行中获取行号
    QModelIndexList list = selectionModel->selectedRows();
    //3.遍历获取每一行的行号
    for(auto item:list)
    {
        //qDebug()<<item.row();
        //根据行号进行删除
        this->wmModel->removeRow(item.row());
    }
    if(!list.isEmpty())
    {
        QString msg("工作模式模块----用户删除了信息");
        emit this->sendOperator(msg);
    }
}

//撤销
void workMode::on_wmReverseBtn_clicked()
{
    //撤销所有的修改，不更新数据库
    this->wmModel->revertAll();

    this->wmModel->submitAll();

}

//查询
void workMode::on_wmSearchBtn_clicked()
{
    int index=ui->wmSelectComBox->currentIndex();
    //qDebug()<<index;
    //1获取用户输入的关键词
    QString key=ui->wmEdit->text();
    QString filterStr;
    //2.拼接模糊查询的条件
    if(index == 0)
    {
        filterStr=QString("peerhost like '%%1%' ").arg(key);
    }
    else if(index == 1) {
        filterStr=QString("port_no like '%%1%' ").arg(key);
    }

    //3.设置过滤
    this->wmModel->setFilter(filterStr);
    //4 重新执行查询
    this->wmModel->select();
}

//排序
void workMode::on_wmSortCombox_currentIndexChanged(int index)
{
    bool ss=ui->upSort->isChecked();
    if(ss)
    {
        if(index == 0)
        {
           ui->tableView->sortByColumn(5,Qt::AscendingOrder);
        }
        else if (index == 1) {
           ui->tableView->sortByColumn(11,Qt::AscendingOrder);
        }
    }
    else
    {
        if(index == 0)
        {
           ui->tableView->sortByColumn(5,Qt::DescendingOrder);
        }
        else if (index == 1) {
           ui->tableView->sortByColumn(11,Qt::DescendingOrder);
        }
    }
}

//升序
void workMode::on_upSort_clicked()
{
    int index=ui->wmSortCombox->currentIndex();
    if(index == 0)
    {
       ui->tableView->sortByColumn(5,Qt::AscendingOrder);
    }
    else if (index == 1) {
       ui->tableView->sortByColumn(11,Qt::AscendingOrder);
    }
}

//降序
void workMode::on_downSort_clicked()
{
    int index=ui->wmSortCombox->currentIndex();
    if(index == 0)
    {
       ui->tableView->sortByColumn(5,Qt::DescendingOrder);
    }
    else if (index == 1) {
       ui->tableView->sortByColumn(11,Qt::DescendingOrder);
    }
}

void workMode::reciveOk(QByteArray buf,QTcpSocket *tcpSocket)
{
    tcpSocket->write(buf);

    qDebug()<<"传回去了！";
}

void workMode::udpReciveOk(QByteArray buf,QUdpSocket *udpSocket,QHostAddress host,quint16 port)
{
    udpSocket->writeDatagram(buf,host,port);
    qDebug()<<"udp传回去了！";
}
