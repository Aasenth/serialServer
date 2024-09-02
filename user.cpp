#include "user.h"

User::User(const QString &username, const QString &password) : username(username)
{
   hashpassword = setHashPassword(password);   //对面密码哈希处理
}

QString User::getUsername() const {
    return username;   //返回用户账户
}

QString User::getPassword() const {
    return hashpassword;    //返回哈希密码
}

QString User::setHashPassword(const QString &password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256); // 使用 SHA-256 算法对密码进行哈希处理
    return hash.toHex(); // 返回哈希值的十六进制表示
}
