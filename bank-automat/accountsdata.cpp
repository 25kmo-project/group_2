#include "accountsdata.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

accountsdata::accountsdata(QObject *parent) : QObject(parent) {
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(4);
    tableModel->setHorizontalHeaderLabels({"TiliID", "KayttäjäID", "Saldo", "Luottoraja"});
}

void accountsdata::setAccountsData(const QByteArray &newAccountsData)
{
    accountsDataList.clear();
    accountsDataArray = newAccountsData;
    QJsonDocument json_doc = QJsonDocument::fromJson(accountsDataArray);

    if(json_doc.isArray()) {
        QJsonArray json_array=json_doc.array();
        for (const QJsonValue &value : json_array) {
            accountsDataList.append(accounts::mapJson(value.toObject()));
        }
    }
    else if (json_doc.isObject()) {
        accountsDataList.append(accounts::mapJson(json_doc.object()));
    }

    //nollataan varmuuden vuoksi
    tableModel->setRowCount(0);

    updateModel();
}

void accountsdata::updateModel()
{
    //tyhjennetään ensin
    tableModel->removeRows(0, tableModel->rowCount());

    for (int row = 0; row < accountsDataList.size(); row++) {
        const accounts &accounts = accountsDataList[row];

        QList<QStandardItem*> rowItems;
        rowItems <<new QStandardItem(QString::number(accounts.idAccount))
                 <<new QStandardItem(accounts.idUser)
                 <<new QStandardItem(QString::asprintf("%.2f €", accounts.balance))
                 <<new QStandardItem(QString::asprintf("%.2f €", accounts.creditLimit));

        rowItems[0]->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        rowItems[1]->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        rowItems[2]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        rowItems[3]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        tableModel->appendRow(rowItems);
    }
 }
