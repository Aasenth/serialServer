#include "maininface.h"
#include "ui_maininface.h"
#include "workmode.h"

#include "serialwidget.h"
#include"initdata.h"
#include "logmanager.h"
#include "userinfo.h"

MainInFace::MainInFace(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainInFace)
{
    ui->setupUi(this);
    this->setWindowTitle("串口服务器");
    this->resize(1200,600);

    InitData *initData = new InitData();
    initData->init_db();
    initData->querySerial();

    QVBoxLayout *wmLayout = new QVBoxLayout(ui->tab_3);
    workMode *workmode =new workMode;
    wmLayout->addWidget(workmode);

    QVBoxLayout *v_log = new QVBoxLayout(ui->tab_2);


    // 串口
    QVBoxLayout *serialLayout = new QVBoxLayout(ui->tab_4);
    SerialWidget *serialWid =new SerialWidget;
    serialLayout->addWidget(serialWid);

     v_log->addWidget(LogManager::getLog());

    QVBoxLayout *userLayout = new QVBoxLayout(ui->tab);
    userinfo *userInfo = new userinfo;
    userLayout->addWidget(userInfo);


}

MainInFace::~MainInFace()
{
    delete ui;
}
