#include "accounts.h"

accounts::accounts() {}

accounts accounts::mapJson(const QJsonObject &json)
{
    accounts accounts;
    accounts.idaccount = json["idaccount"].toInt();
    accounts.iduser = json["iduser"].toString();
    accounts.balance = json["balance"].toDouble();
    accounts.creditlimit = json["creditlimit"].toDouble();
    return accounts;

}
