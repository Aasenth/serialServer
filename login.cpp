#include "login.h"
#include "ui_login.h"
#include "maininface.h"
#include<QPushButton>
#include<QMessageBox>
#include<QLineEdit>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QPainter>

#include "logmanager.h"

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);

    //去掉标题栏
    //setWindowFlag(Qt::FramelessWindowHint);

    //固定大小
    setFixedSize(520,350);

    //设置密码框为密文
    ui->passEdit->setEchoMode(QLineEdit::Password);


    this->mainInface = new MainInFace;
    connect(this,SIGNAL(send(QString)),LogManager::getLog(),SLOT(recive(QString)));
    connect(this,SIGNAL(sendOperator(QString)),LogManager::getLog(),SLOT(reciveOperator(QString)));

}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *event)
{
    //实例化画家
    QPainter painter(this);

    //设置背景为黑色
    painter.setBrush(Qt::white);
    painter.drawRect(0,0, width(), height()); //填充窗口

}

void Login::on_LoginBtn_clicked()
{
    QString username = ui->userEdit->text();
    QString password = ui->passEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Error","账号密码不能为空");
        return;

        //QMessageBox::warning(this, "Error","账号密码不能为空");
        //return;

    }

    //哈希输入的密码
    User user(username,password);
    QString hashpassword = user.getPassword();


    //查询数据库连接
    if (!QSqlDatabase::database().isOpen()) {
            QMessageBox::critical(this, "Error","执行查询失败，请确认数据库"); // 执行查询失败，显示错误信息
            return;
        }

    //查询数据库中是否存在匹配的用户
    QString sql = QString("SELECT * FROM userinfo WHERE username='%1' and password_hash='%2'").arg(username).arg(hashpassword);
    QSqlQuery query(sql);

      if (query.next()) { // 如果找到匹配的用户名
                 QMessageBox::information(this, "OK", "登陆成功, " + username + "!"); // 登录成功时，显示欢迎消息
                 // 登录成功，跳转页面

                 this->mainInface->show();
                 this->hide();

                 QString msg(QString("用户%1登录").arg(username));
                 emit this->send(msg);
                 emit this->sendOperator(msg);
             } else
             {
                 QMessageBox::critical(this, "Error", "账户或密码错误，请确认输入是否正确.");
             }

}
