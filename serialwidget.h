#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include<QTcpSocket>
#include<QTcpServer>
namespace Ui {
class SerialWidget;
}

class SerialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SerialWidget(QWidget *parent = nullptr);
    ~SerialWidget();
     void initSerial();


signals:
    void send(QString msg);
    void sendOperator(QString msg);

private slots:
    void on_addSerialBtn_clicked();

    void on_delSerialBtn_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::SerialWidget *ui;

    QSqlRelationalTableModel *serialModel;
};

#endif // SERIALWIDGET_H
