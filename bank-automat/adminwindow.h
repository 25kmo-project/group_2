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
    explicit adminwindow(int idAccount, const QString& idUser, const QString& fName, ApiClient* api, QWidget *parent = nullptr);
    ~adminwindow();

private:
    Ui::adminwindow *ui;
    //QString user;
    ApiClient* m_api = nullptr;
    int m_idAccount = 0;
    QString m_idUser;
    QString m_fName;

    class userdata *userData;
    class accountsdata *accountsData;
    class carddata *cardData;
    class logs *logData;

    QByteArray testUserData;
    QByteArray testAccountsData;
    QByteArray testCardData;
    QByteArray testLogData;

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
};

#endif // ADMINWINDOW_H
