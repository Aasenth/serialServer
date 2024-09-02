#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QWidget>

namespace Ui {
class LogManager;
}

class LogManager : public QWidget
{
    Q_OBJECT

public:
    static LogManager* getLog();
    ~LogManager();

    void WriteFileLog();
    void WriteOperatorLog();


private slots:
    void recive(QString msg);
    void reciveOperator(QString msg);

private:
    explicit LogManager(QWidget *parent = nullptr);

    static LogManager* log;
    Ui::LogManager *ui;
    QString nowTime;
    QString nowMsg = "";  //网络信息
    QString nowOperatorMsg = "";  //操作信息

};

#endif // LOGMANAGER_H
