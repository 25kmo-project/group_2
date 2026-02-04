#ifndef CARDDATA_H
#define CARDDATA_H

#include "apiclient.h"
#include "card.h"
#include <QObject>
#include <QVector>
#include <QStandardItemModel>

class carddata : public QObject
{
    Q_OBJECT
public:
    explicit carddata(ApiClient* api, QObject * parent = nullptr);
    void setCardData(const QByteArray &newCardData);
    QStandardItemModel* getModel() const {return tableModel;};

    void updateModel();

private:
    QByteArray cardDataArray;
    QVector<card> cardDataList;
    QStandardItemModel *tableModel;

    ApiClient* m_api = nullptr;

private slots:
    void linkCardAccount(const QByteArray linkData);
};

#endif // CARDDATA_H
