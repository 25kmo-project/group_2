#ifndef LOGS_H
#define LOGS_H

#include "logevents.h"
#include <QObject>
#include <QVector>
#include <QStandardItemModel>

class logs : public QObject
{
    Q_OBJECT

public:
    explicit logs(QObject *parent = nullptr);
    void setLog(const QByteArray &newLog);
    QStandardItemModel* getModel() const { return tableModel;};

    void updateModel();
    void nextPage();
    void prevPage();

private:
    QByteArray logArray;
    QVector<logevents> loglist;
    QStandardItemModel *tableModel;
    int currentPage;
};

#endif // LOGS_H
