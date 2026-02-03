#ifndef CARD_H
#define CARD_H

#include <QString>
#include <QJsonObject>

class card
{
public:
    card();
    QString idCard;
    QString idUser;
    int isLocked;
    int pinAttempst;

    static card mapJson(const QJsonObject &json);
};

#endif // CARD_H
