#include "logs.h"
#include "logevents.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

logs::logs(QObject *parent) : QObject(parent) {
    tableModel = new QStandardItemModel(this);
    tableModel->setRowCount(0);
    tableModel->setColumnCount(3);
    tableModel->setHorizontalHeaderLabels({"ID", "Aika", "Muutos"});
}

void logs::setLog(const QByteArray &newLog)
{
    loglist.clear();
    logArray = newLog;
    QJsonDocument json_doc = QJsonDocument::fromJson(logArray);
    QJsonArray json_array=json_doc.array();

    //QVector<logevents> loglist;
    for (const QJsonValue &value : json_array) {
        //jsonista c++ objektiksi
        if (value.isObject()) {
            logevents logev = logevents::mapJson(value.toObject());
            loglist.append(logev);
        }
    }
    //nollataan varmuuden vuoksi
    tableModel->setRowCount(0);
    currentPage = 0;

    updateModel();
}

//Tehdään uusi metodi, kun tarvitaan sekä aloituksessa että päivittäessä, ei tule toistoa
void logs::updateModel()
{
    //tyhjennetään ensin
    tableModel->removeRows(0, tableModel->rowCount());
    int start = currentPage * 10;   //Mistä aloitetaan lisääminen
    int end = qMin(start + 10, (int)loglist.size()); //Loppuu joko 10 tai jos lista loppuu (int) varmistaa että samaa muotoa

    for (int i = start; i < end; i++) {
        const logevents &event = loglist[i];
        QStandardItem *idlog = new QStandardItem(QString::number(event.idlog));
        idlog->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        QStandardItem *aika = new QStandardItem(event.time);
        aika->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        QStandardItem *muutos = new QStandardItem(QString::asprintf("%.2f €", event.balancechange));
        muutos->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableModel->appendRow({idlog, aika, muutos});
    }
    //Lisätää tyhjiä rivejä että päästään tasan 10. Helpottaa muotoilua
    int currentRows = tableModel->rowCount();
    for (int i = currentRows; i < 10; i++) {
        QStandardItem *idlogTyhja = new QStandardItem("");
        QStandardItem *aikaTyhja = new QStandardItem("");
        QStandardItem *muutosTyhja = new QStandardItem(QString::asprintf(""));
        tableModel->appendRow({idlogTyhja, aikaTyhja, muutosTyhja});
    }
}

void logs::nextPage()
{
    //ceil pyöristää ylöspäin, -1 koska indeksi alkaa nollasta. 10.0 että osaa säilyttä desimaalit eikä pyöristä heti alaspäin
    if (currentPage < ceil(loglist.size()/10.0) -1 ) {
        currentPage++;
        updateModel();
    }
}

void logs::prevPage()
{
    if (currentPage > 0) {
        currentPage--;
        updateModel();
    }
}
