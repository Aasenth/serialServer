#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "user.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QCryptographicHash>


#include "maininface.h"
#include "logmanager.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:
    void send(QString msg);
    void sendOperator(QString msg);


private slots:
    void on_LoginBtn_clicked();

private:
    Ui::Login *ui;


    MainInFace *mainInface;

protected:
    void paintEvent(QPaintEvent *event);

};

#endif // LOGIN_H
