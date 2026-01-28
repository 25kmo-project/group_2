#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

class user
{
public:
    user();
    QString iduser;
    QString fname;
    QString lname;
    QString streetaddress;
    static user mapJson(const QJsonObject &json);
};

#endif // USER_H
