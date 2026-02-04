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
    QList<int> linkedCardAccount;
    int isLocked;
    int pinAttempts;

    static card mapJson(const QJsonObject &json);
};

#endif // CARD_H
