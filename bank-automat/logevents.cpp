#include "logevents.h"
#include <QDateTime>
#include <QLocale>

logevents::logevents() {}

logevents logevents::mapJson(const QJsonObject &json)
{
    logevents logevent;
    logevent.idlog = json["idlog"].toInt();
    logevent.idaccount = json["idaccount"].toInt();
    
    // Parse UTC timestamp from backend
    QString utcTimeStr = json["time"].toString();
    QDateTime utcTime = QDateTime::fromString(utcTimeStr, Qt::ISODate);
    utcTime.setTimeSpec(Qt::UTC);
    
    // Convert to local time and format nicely
    QDateTime localTime = utcTime.toLocalTime();
    logevent.time = localTime.toString("dd.MM.yyyy HH:mm:ss");
    
    logevent.balancechange = json["balancechange"].toDouble();
    return logevent;
}
