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
    QJsonArray json_array=json_doc.array();

    //QVector<accounts> loglist;
    for (const QJsonValue &value : json_array) {
        //jsonista c++ objektiksi
        if (value.isObject()) {
            accounts accountsData = accounts::mapJson(value.toObject());
            accountsDataList.append(accountsData);
        }
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
        tableModel->setItem(row,0, new QStandardItem(accounts.idaccount));
        tableModel->setItem(row,1, new QStandardItem(accounts.iduser));
        tableModel->setItem(row,2, new QStandardItem(accounts.balance));
        tableModel->setItem(row,3, new QStandardItem(accounts.creditlimit));
    }
}
