#include "userinfo.h"
#include "ui_userinfo.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>
#include <QSqlRelationalDelegate>
#include <QSqlQuery>
#include <QDialog>
#include <QFormLayout>
#include <QPushButton>
#include <QLineEdit>
#include "logmanager.h"

userinfo::userinfo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::userinfo)
{
    ui->setupUi(this);
    this->resize(1200,600);



        // 设置列数
        ui->tableWidget->setColumnCount(5);

        // 设置表头
        QStringList headers;
        headers << "ID" << "Username" << "Password" << "Edit" << "Delete";
        ui->tableWidget->setHorizontalHeaderLabels(headers);

        ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 新增用户
        connect(ui->addBtn, &QPushButton::clicked, this, &userinfo::openAddDialog);


        //发送信号记录日志
        connect(this,SIGNAL(sendOperator(QString)),LogManager::getLog(),SLOT(reciveOperator(QString)));

        //搜索按钮点击事件
        connect(ui->ssBtn, &QPushButton::clicked, this, &userinfo::on_ssBtn_clicked);


        // 设置行高自动调整
        //ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);



    QString query=QString(" SELECT  id,username, password_hash FROM userinfo");
        QSqlQuery querry(query);
        int row = 0;
            while (querry.next()) {
                // 插入新行
                ui->tableWidget->insertRow(row);

                // 获取每一列的数据
                int ids = querry.value(0).toInt();
                QString usname = querry.value(1).toString();
                QString passwordHash = querry.value(2).toString();

                // 将数据添加到表格中
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(ids)));
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem(usname));
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem(passwordHash));

                //创建编辑按钮
                QPushButton *editBtn = new QPushButton("编辑");

                connect(editBtn, &QPushButton::clicked, [this,ids](){
                    this->openEditDialog(ids);
                });
                ui->tableWidget->setCellWidget(row,3,editBtn);
                //editBtn->setFixedSize(100,80);

                //创建删除
                QPushButton *deleteBtn = new QPushButton("删除");

                connect(deleteBtn, &QPushButton::clicked, [this,ids](){
                    this->deleteUser(ids);
                });
                ui->tableWidget->setCellWidget(row,4,deleteBtn);
                //deleteBtn->setFixedSize(100,80);

                row++;
    }

}

userinfo::~userinfo()
{
    delete ui;
}


//处理编辑
void userinfo::openEditDialog(int userId)
{
    //创建编辑对话框
    QDialog editDialog(this);
    QFormLayout layout(&editDialog);

    // 获取当前用户的数据
    QSqlQuery query;
    query.prepare("SELECT username, password_hash FROM userinfo WHERE id = ?");
    query.addBindValue(userId);
    query.exec();
    query.next();
    QString currentUsername = query.value(0).toString();
    QString currentPasswordHash = query.value(1).toString();

    // 创建输入框
    QLineEdit *usernameEdit = new QLineEdit(currentUsername);
    QLineEdit *passwordEdit = new QLineEdit(currentPasswordHash);
    layout.addRow("用户名:", usernameEdit);
    layout.addRow("密码哈希:", passwordEdit);

    // 创建确认按钮
        QPushButton *qrBtn = new QPushButton("确认");
        connect(qrBtn, &QPushButton::clicked, [&]() {
        QString newUsername = usernameEdit->text(); //获取新用户名
        QString newPassword = passwordEdit->text(); //获取新密码
        QString newHashPassword = User::setHashPassword(newPassword);
        //更新信息
        updateUser(userId, newUsername, newHashPassword);
        editDialog.accept();  //关闭对话框
        QString msg(QString("用户管理模块----修改当前用户%1的信息").arg(currentUsername));
        emit this->sendOperator(msg);
        });
        layout.addWidget(qrBtn);

        // 显示对话框
        editDialog.exec();
}


//处理删除
void userinfo::deleteUser(int userId) {

    QSqlQuery query1;
    query1.prepare("SELECT username FROM userinfo WHERE id = ?");
    query1.addBindValue(userId);
    query1.exec();
    query1.next();
    QString currentUsername = query1.value(0).toString();
    QString msg(QString("用户管理模块----删除了用户%1").arg(currentUsername));
    emit this->sendOperator(msg);

    // 删除用户
    QSqlQuery query;
    query.prepare("DELETE FROM userinfo WHERE id = ?");
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "删除失败：" << query.lastError().text();
    } else {
        // 删除成功，刷新表格
        refreshTable();
    }
}

//处理新增
void userinfo::openAddDialog() {
    // 创建新增对话框
    QDialog addDialog(this);
    QFormLayout layout(&addDialog);

    // 创建输入框
    QLineEdit *usernameEdit = new QLineEdit();
    QLineEdit *passwordEdit = new QLineEdit();
    layout.addRow("用户名:", usernameEdit);
    layout.addRow("密码:", passwordEdit);

    // 创建确认按钮
    QPushButton *xzqrBtn = new QPushButton("确认");
    connect(xzqrBtn, &QPushButton::clicked, [&]() {
        QString newUsername = usernameEdit->text(); // 获取新用户名
        QString newPassword = passwordEdit->text(); // 获取新密码
        QString newHashPassword = User::setHashPassword(newPassword); // 使用静态方法哈希密码

        // 添加新用户
        addUser(newUsername, newHashPassword);
        addDialog.accept(); // 关闭对话框
        QString msg(QString("用户管理模块----添加了用户%1").arg(newUsername));
        emit this->sendOperator(msg);
    });
    layout.addWidget(xzqrBtn);

    // 显示对话框
    addDialog.exec();
}

//处理新增用户
void userinfo::addUser(const QString &username, const QString &passwordHash) {
    QSqlQuery query;
    query.prepare("INSERT INTO userinfo (username, password_hash) VALUES (?, ?)");
    query.addBindValue(username);
    query.addBindValue(passwordHash);

    if (!query.exec()) {
        qDebug() << "新增失败：" << query.lastError().text();
    } else {
        // 新增成功，刷新表格
        refreshTable();
    }
}


//处理更新
void userinfo::updateUser(int userId, const QString &username, const QString &passwordHash) {
    QSqlQuery query;
    query.prepare("UPDATE userinfo SET username = ?, password_hash = ? WHERE id = ?");
    query.addBindValue(username);
    query.addBindValue(passwordHash); // 传递哈希后的密码
    query.addBindValue(userId);

    if (!query.exec()) {
        qDebug() << "更新失败：" << query.lastError().text();
    } else {
        // 更新成功，刷新表格
        refreshTable();
    }
}
//处理更新
void userinfo::refreshTable() {
    // 清空当前表格内容
    ui->tableWidget->setRowCount(0);

    // 重新查询并填充表格
    QString queryStr = "SELECT id, username, password_hash FROM userinfo";
    QSqlQuery query(queryStr);
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row);

        int ids = query.value(0).toInt();
        QString usname = query.value(1).toString();
        QString passwordHash = query.value(2).toString();

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(ids)));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(usname));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(passwordHash));

        QPushButton *editBtn = new QPushButton("编辑");

        //编辑按钮点击事件
        connect(editBtn, &QPushButton::clicked, [this, ids]() {
            this->openEditDialog(ids);
        });

        ui->tableWidget->setCellWidget(row, 3, editBtn);
        //editBtn->setFixedSize(100,80);

        QPushButton *deleteBtn = new QPushButton("删除");

        //删除按钮点击事件
        connect(deleteBtn, &QPushButton::clicked, [this, ids]() {
            this->deleteUser(ids);
        });
        ui->tableWidget->setCellWidget(row, 4, deleteBtn);
        //deleteBtn->setFixedSize(100,80);

        row++;
    }
}

//查询
void userinfo::on_ssBtn_clicked()
{
    //获取 textedit 中输入的内容并去掉空白字符
    QString editText = ui->textEdit->toPlainText().trimmed();

    //遍历数据库行列获取数据库的内容
    for (int row = 0; row < ui->tableWidget->rowCount(); row++)
    {
        bool flag = false;

        //获取每一行的 username 内容
        QTableWidgetItem *item = ui->tableWidget->item(row,1);
            if (item)
            {
                //如果有某个单元格的内容包含搜索框的关键词，则该行匹配
                    flag = item->text().contains(editText,Qt::CaseInsensitive);
            }
        //根据是否有可以匹配的内容，显示该行
        ui->tableWidget->setRowHidden(row,!flag);
    }
}

//刷新按钮
void userinfo::on_addBtn_2_clicked()
{
    refreshTable();
}
