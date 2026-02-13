#ifndef USERDATA_H
#define USERDATA_H

#include "user.h"
#include <QObject>
#include <QVector>
#include <QStandardItemModel>

class userdata : public QObject
{
    Q_OBJECT

public:
    explicit userdata(QObject *parent = nullptr);
    void setUserData(const QByteArray &newUserData);
    QStandardItemModel* getModel() const { return tableModel;};

    void updateModel();
    int findRow(QString idUser);

private:
    QByteArray userDataArray;
    QVector<user> userDataList;
    QStandardItemModel *tableModel;
};

#endif // USERDATA_H
