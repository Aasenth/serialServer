#include "serialwidget.h"
#include "ui_serialwidget.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include<QSqlRecord>
#include "logmanager.h"

SerialWidget::SerialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialWidget)
{
    ui->setupUi(this);
    this->initSerial();

    //链接信号书写日志
    connect(this,SIGNAL(send(QString)),LogManager::getLog(),SLOT(recive(QString)));
    connect(this,SIGNAL(sendOperator(QString)),LogManager::getLog(),SLOT(reciveOperator(QString)));
}

SerialWidget::~SerialWidget()
{
    delete ui;
}

// 初始化串口数据
void SerialWidget:: initSerial()
{

    // 2.  启动程序时，在表格中显示表的数据
    this->serialModel = new QSqlRelationalTableModel(this);
    // 设置要管理的数据表的表名
    this->serialModel->setTable("serials");
    // 从 student 表中查询出所有数据
    this->serialModel->select();
    // 将查询结果显示在tableView当中 (将模型中的数据设置到表格中)
    ui->serialTableView->setModel(this->serialModel);

    // 注意: 查询完成之后才能设置表头
    this->serialModel->setHeaderData(0, Qt::Horizontal, "id");
    this->serialModel->setHeaderData(1, Qt::Horizontal, "端口");
    this->serialModel->setHeaderData(2, Qt::Horizontal, "串口号");
    this->serialModel->setHeaderData(3, Qt::Horizontal, "串口名");
    this->serialModel->setHeaderData(4, Qt::Horizontal, "波特率");
    this->serialModel->setHeaderData(5, Qt::Horizontal, "数据位");
    this->serialModel->setHeaderData(6, Qt::Horizontal, "停止位");
    this->serialModel->setHeaderData(7, Qt::Horizontal, "校验位");
    this->serialModel->setHeaderData(8, Qt::Horizontal, "流控制");
    this->serialModel->setHeaderData(9, Qt::Horizontal, "接收空闲时间");
    this->serialModel->setHeaderData(10, Qt::Horizontal, "接收空闲时间");

    // 隔行变色
    ui->serialTableView->setAlternatingRowColors(true);
    //ui->serialTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


    // 设置表格的修改属性
    // OnFieldChange 对模型的所有更改将立即应用于数据库。
    // OnRowChange: 当用户选择另一行时，将应用对一行的更改。
    // OnManualSubmit:在调用submitAll()或revertAll()之前，所有更改都将缓存在模型中。
    this->serialModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
}

// 添加数据
void SerialWidget:: on_addSerialBtn_clicked()
{
    //1.创建一个空行 (此时，该空行和模型还没有关系，只是内存中创建出一个空行)
    QSqlRecord record = this->serialModel->record();

    //2获取模型的总行书
    int num=this->serialModel->rowCount();

    //将创建的空行，添加到model的最后一行
    this->serialModel->insertRecord(num,record);

    QString msg("串口管理模块-----添加了信息");
    emit this->sendOperator(msg);


}

// 删除数据
void SerialWidget::on_delSerialBtn_clicked()
{
    QItemSelectionModel *selectedModel = ui->serialTableView->selectionModel();

    // 获取选中行的行号
    QModelIndexList indexList = selectedModel->selectedRows();

    if(!indexList.isEmpty())
    {
        QString msg("串口管理模块----用户删除了信息");
        emit this->sendOperator(msg);
    }

    for (int i = 0; i < indexList.size(); i++)
    {
        qDebug() << i << indexList[i].row();
        qDebug() << i << indexList[i].data(1);
        serialModel->removeRow(indexList[i].row());
    }

}

void SerialWidget::on_pushButton_3_clicked()
{
    this->serialModel->submitAll();
}


void SerialWidget::on_pushButton_clicked()
{
    //撤销所有的修改，不更新数据库
    this->serialModel->revertAll();

    this->serialModel->submitAll();
}
