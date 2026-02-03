#include "userdata.h"
#include "user.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

userdata::userdata(QObject *parent) : QObject(parent) {
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(5);
    tableModel->setHorizontalHeaderLabels({"KäyttäjäID", "Etunimi", "Sukunimi", "Osoite", "Rooli"});
}

void userdata::setUserData(const QByteArray &newUserData)
{
    userDataList.clear();
    userDataArray = newUserData;
    QJsonDocument json_doc = QJsonDocument::fromJson(userDataArray);

    if (json_doc.isArray()) {
        QJsonArray json_array = json_doc.array();
        for (const QJsonValue &value : json_array) {
            userDataList.append(user::mapJson(value.toObject()));
        }
    }
    else if (json_doc.isObject()) {
        userDataList.append(user::mapJson(json_doc.object()));
    }

    tableModel->setRowCount(0);

    updateModel();
}

void userdata::updateModel()
{
    //tyhjennetään ensin
    tableModel->removeRows(0, tableModel->rowCount());

    for (int row = 0; row < userDataList.size(); row++) {
        const user &user = userDataList[row];

        QList<QStandardItem*> rowItems;
        rowItems <<new QStandardItem(user.idUser)
                 <<new QStandardItem(user.firstName)
                 <<new QStandardItem(user.lastName)
                 <<new QStandardItem(user.streetAddress)
                 <<new QStandardItem(user.role);

        for (auto item : rowItems) {
            item->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        }

        tableModel->appendRow(rowItems);
    }
}
