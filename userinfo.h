#ifndef USERINFO_H
#define USERINFO_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include "user.h"

namespace Ui {
class userinfo;
}

class userinfo : public QWidget
{
    Q_OBJECT

public:
    explicit userinfo(QWidget *parent = nullptr);
    ~userinfo();
signals:
    void sendOperator(QString msg);

private slots:

    //void on_userSearchBtn_clicked();

    void openEditDialog(int userId);

    void openAddDialog();

    void addUser(const QString &username, const QString &passwordHash);

    void updateUser(int userId, const QString &username, const QString &passwordHash);

    void deleteUser(int userId);

    void refreshTable();


    void on_ssBtn_clicked();

    void on_addBtn_2_clicked();

private:
    Ui::userinfo *ui;
    QSqlRelationalTableModel *userModel;

};

#endif // USERINFO_H
