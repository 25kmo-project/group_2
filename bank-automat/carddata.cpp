#include "carddata.h"
#include "apiclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

carddata::carddata(ApiClient *api, QObject *parent) : QObject(parent)
    ,m_api(api)
{
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(5);
    tableModel->setHorizontalHeaderLabels({"KortinID", "KayttäjäID", "Tilit", "Lukossa", "PIN yrityksiä"});

    connect(m_api, &ApiClient::cardAccountReceived, this, &carddata::linkCardAccount);
}

void carddata::setCardData(const QByteArray &newCardData)
{
    cardDataList.clear();
    cardDataArray = newCardData;
    QJsonDocument json_doc = QJsonDocument::fromJson(cardDataArray);

    if (json_doc.isArray()) {
        QJsonArray json_array = json_doc.array();
        for (const QJsonValue &value : json_array) {
            card c = card::mapJson(value.toObject());
            cardDataList.append(c);
            m_api->getCardAccount(c.idCard);
        }

    }
    else if (json_doc.isObject()) {
        card cardObj = card::mapJson(json_doc.object());
        cardDataList.append(cardObj);
        m_api->getCardAccount(cardObj.idCard);
    }

    tableModel->setRowCount(0);

    updateModel();
}

void carddata::linkCardAccount(const QByteArray linkData)
{
    //qDebug() << "Step 1: Link data received: " << linkData;
    //Receive linkdata
    QJsonDocument doc = QJsonDocument::fromJson(linkData);
    //Check if single account or multiple and standardizes format
    QJsonArray array;
    if (doc.isObject()) {
        QJsonObject root = doc.object();
        if (root.contains("data") && root.value("data").isArray()) {
            array = root.value("data").toArray();
        }
        else {
            array.append(root);
        }
    }
    else if (doc.isArray()) {
        array = doc.array();
    }
    else {
        array.append(doc.object());
    }
    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        QString linkIdCard = obj.value("idCard").toString();
        int linkIdAccount = obj.value("idAccount").toInt();
        //qDebug() << "Step 2: Looking for Card ID" << linkIdCard;
        for (card &c : cardDataList) {
            if (c.idCard == linkIdCard) {
                //qDebug() << "Step 3: Found Match! Adding Account" << linkIdAccount;
                //Just in case does not accept duplicates
                if (!c.linkedCardAccount.contains(linkIdAccount)) {
                    c.linkedCardAccount.append(linkIdAccount);
                }
            }
        }
    }

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
        QString accounts;
        for (int idAccount : card.linkedCardAccount) {
            accounts += QString::number(idAccount) + "  ";
        }
        if (accounts.isEmpty()) {
            accounts = "-";
        }
        QStandardItem *accountsItem = new QStandardItem(accounts);
        accountsItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 2, accountsItem);
        QString islockedString = card.isLocked ? "True" : "False";
        QStandardItem *lockedItem = new QStandardItem(islockedString);
        lockedItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 3, lockedItem);
        QStandardItem *pinItem = new QStandardItem(QString::number(card.pinAttempts));
        pinItem->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableModel->setItem(row, 4, pinItem);
    }
}




