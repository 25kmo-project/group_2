#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <QString>
#include <QJsonObject>

class accounts
{
public:
    accounts();
    int idaccount;
    QString iduser;
    double balance;
    double creditlimit;
    static accounts mapJson(const QJsonObject &json);
};

#endif // ACCOUNTS_H
