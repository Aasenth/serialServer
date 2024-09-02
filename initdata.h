#ifndef INITDATA_H
#define INITDATA_H

#include<QSqlDatabase>
#include <QObject>
#include "serial.h"
#include<QMap>
class InitData : public QObject
{
    Q_OBJECT
public:
    explicit InitData(QObject *parent = nullptr);
    // 数据库链接
    void init_db();
    // 查询窗口配置表
    void querySerial();
    // 获取配置表信息
    void getSerial(QSqlQuery query);

    static QMap<int,Serial*> serialMap;

signals:
private:

    //Serial *serial;


public slots:
};

#endif // INITDATA_H
