#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include "apiclient.h"
#include <QWidget>

namespace Ui {
class adminwindow;
}

class adminwindow : public QWidget
{
    Q_OBJECT

public:
    explicit adminwindow(QString idUser, ApiClient* api, QWidget *parent = nullptr);

    ~adminwindow();

private:
    Ui::adminwindow *ui;
    //QString user;
    ApiClient* m_api = nullptr;
    QString m_idUser;
    int waitingAccountId = 0;

    class userdata *userData;
    class accountsdata *accountsData;
    class carddata *cardData;
    class logs *logData;

    QByteArray testUserData;
    QByteArray testAccountsData;
    QByteArray testCardData;
    QByteArray testLogData;

    void clearAsiakkaatInputs();
    void clearTilitInputs();
    void clearKortitInputs();

protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void on_btnAsiakkaatLowBar_clicked();
    void on_btnTilitLowBar_clicked();
    void on_btnKortitLowBar_clicked();
    void on_btnLokitHae_clicked();
    void on_btnLokitLowBar_clicked();
    void on_btnLogOutLowBar_2_clicked();
    void on_btnKayttajaLuoUusi_clicked();
    void on_btnKayttajaHae_clicked();
    void on_btnKayttajaPaivitaTietoja_clicked();
    void on_btnKayttajaPoista_clicked();
    void on_btnTiliHae_clicked();
    void on_btnTiliLuoUusi_clicked();
    void on_btnTiliPaivitaLuottoraja_clicked();
    void on_btnTiliPoistaTili_clicked();
    void on_btnKayttajaHaeKaikki_clicked();
    void on_btnTiliHaeKaikki_clicked();
    void on_btnKorttiLuoUusi_clicked();
    void on_btnKorttiHaeKaikki_clicked();
    void on_btnKorttiHaeKortti_clicked();
    void on_btnKorttiPoista_clicked();
    void on_btnKorttiPaivitaPIN_clicked();
    void on_btnKorttiLukitse_clicked();
    void on_btnKorttiAvaa_clicked();
    void on_btnKorttiLinkita_clicked();
    void on_btnKorttiPoistaLinkitys_clicked();
    void on_btnKorttiPaivitaLinkitys_clicked();
};

#endif // ADMINWINDOW_H
