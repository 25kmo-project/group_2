#include "userdata.h"
#include "user.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

userdata::userdata(QObject *parent) : QObject(parent) {
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(4);
    tableModel->setHorizontalHeaderLabels({"KäyttäjäID", "Etunimi", "Sukunimi", "Osoite"});
}

void userdata::setUserData(const QByteArray &newUserData)
{
    userDataList.clear();
    userDataArray = newUserData;
    QJsonDocument json_doc = QJsonDocument::fromJson(userDataArray);
    QJsonArray json_array=json_doc.array();

    //QVector<user> loglist;
    for (const QJsonValue &value : json_array) {
        //jsonista c++ objektiksi
        if (value.isObject()) {
            user userData = user::mapJson(value.toObject());
            userDataList.append(userData);
        }
    }
    //nollataan varmuuden vuoksi
    tableModel->setRowCount(0);

    updateModel();
}

void userdata::updateModel()
{
    //tyhjennetään ensin
    tableModel->removeRows(0, tableModel->rowCount());

    for (int row = 0; row < userDataList.size(); row++) {
        const user &user = userDataList[row];
        tableModel->setItem(row,0, new QStandardItem(user.iduser));
        tableModel->setItem(row,1, new QStandardItem(user.fname));
        tableModel->setItem(row,2, new QStandardItem(user.lname));
        tableModel->setItem(row,3, new QStandardItem(user.streetaddress));
    }
}
