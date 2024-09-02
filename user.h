#ifndef USER_H
#define USER_H
#include<QString>
#include<QCryptographicHash>

class User
{
public:
    //构造函数初始化用户名密码
    User(const QString &username, const QString &password);
    QString getUsername() const;   //获取用户名
    QString getPassword() const;   //获取密码
    static QString setHashPassword(const QString &password);  //密码进行哈希处理

private:
    QString username;
    QString hashpassword;


};

#endif // USER_H
