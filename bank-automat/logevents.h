#ifndef LOGEVENTS_H
#define LOGEVENTS_H

#include <QString>
#include <QJsonObject>

class logevents
{
public:
    logevents();
    int idlog;
    int idaccount;
    QString time;
    double balancechange;
    static logevents mapJson(const QJsonObject &json);
};

#endif // LOGEVENTS_H
