#include "user.h"

user::user() {}

user user::mapJson(const QJsonObject &json)
{
    user user;
    user.iduser = json["iduser"].toString();
    user.fname = json["fname"].toString();
    user.lname = json["lname"].toString();
    user.streetaddress = json["streetaddress"].toString();
    return user;
}
