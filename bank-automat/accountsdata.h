#ifndef ACCOUNTSDATA_H
#define ACCOUNTSDATA_H

#include "accounts.h"
#include <QObject>
#include <QVector>
#include <QStandardItemModel>

class accountsdata : public QObject
{
    Q_OBJECT

public:
    explicit accountsdata(QObject *parent = nullptr);
    void setAccountsData(const QByteArray &newAccountsData);
     QStandardItemModel* getModel() const { return tableModel;};

    void updateModel();

private:
    QByteArray accountsDataArray;
    QVector<accounts> accountsDataList;
    QStandardItemModel *tableModel;
};

#endif // ACCOUNTSDATA_H
