#include "logs.h"
#include "logevents.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

logs::logs(QObject *parent) : QObject(parent) {}

void logs::setLog(const QByteArray &newLog)
{
    logArray = newLog;
    QJsonDocument json_doc = QJsonDocument::fromJson(logArray);
    QJsonArray json_array=json_doc.array();

    QVector<logevents> loglist;
    for (const QJsonValue &value : json_array) {
        if (value.isObject()) {
            logevents logev = logevents::mapJson(value.toObject());
            loglist.append(logev);
        }
    }

    tableModel->setRowCount(0);
    tableModel->setColumnCount(3);
    tableModel->setHorizontalHeaderLabels({"ID", "Aika", "Muutos"});

    for (int row = 0; row < loglist.size(); row ++) {
        const logevents &event = loglist[row];

        QStandardItem *ID = new QStandardItem(QString::number(event.idlog));
        QStandardItem *aika = new QStandardItem(event.time);
        aika->setTextAlignment(Qt::AlignRight);
        QStandardItem *muutos = new QStandardItem(QString::asprintf("%.2f €", event.balancechange));

        tableModel->appendRow({ID, aika, muutos});
    }
}
