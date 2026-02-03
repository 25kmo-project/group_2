#include "carddata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

carddata::carddata(QObject *parent) : QObject(parent){
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(4);
    tableModel->setHorizontalHeaderLabels({"KortinID", "KayttäjäID", "Lukossa", "PIN yrityksiä"});
}

void carddata::setCardData(const QByteArray &newCardData)
{
    cardDataList.clear();
    cardDataArray = newCardData;
    QJsonDocument json_doc = QJsonDocument::fromJson(cardDataArray);

    if (json_doc.isArray()) {
        QJsonArray json_array = json_doc.array();
        for (const QJsonValue &value : json_array) {
            cardDataList.append(card::mapJson(value.toObject()));
        }
    }
    else if (json_doc.isObject()) {
        cardDataList.append(card::mapJson(json_doc.object()));
    }

    tableModel->setRowCount(0);

    updateModel();
}

void carddata::updateModel()
{
    //tyhjennetään ensin
    tableModel->removeRows(0, tableModel->rowCount());

    for (int row = 0; row < cardDataList.size(); row++) {
        const card &card = cardDataList[row];
        QStandardItem *idCardItem = new QStandardItem(card.idCard);
        idCardItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 0, idCardItem );
        QStandardItem *idUserItem = new QStandardItem(card.idUser);
        idUserItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 1 , idUserItem);
        QString islockedString = card.isLocked ? "True" : "False";
        QStandardItem *lockedItem = new QStandardItem(islockedString);
        lockedItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 2, lockedItem);
        QStandardItem *pinItem = new QStandardItem(QString::number(card.pinAttempst));
        pinItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 3, pinItem);
    }
}


