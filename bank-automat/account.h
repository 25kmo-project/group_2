#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
#include <QStandardItemModel>   //tableviewiä varten
//Tarvitaan hard resettiin:
#include <QProcess>
#include <QCoreApplication>
#include <QDialog>
#include <QLineEdit>
#include <QTimer>
#include <QLabel>
#include <QKeyEvent>
#include "logs.h"
#include "apiclient.h"

namespace Ui {
class account;
}

class account : public QDialog
{
    Q_OBJECT

public:
    explicit account(int idAccount, const QString& idUser, const QString& fName, ApiClient* api, QWidget *parent = nullptr);
    ~account();

private:
    Ui::account *ui;
    ApiClient* m_api = nullptr;
    int m_idAccount = 0;
    QString m_idUser;
    QString m_fName;
    logs* tapahtumat = nullptr;
    double saldo = 0.0;
    double creditlimit = 0.0;
    double nostosumma = 0.0;

    // Withdrawal validation
    // Valid amounts are: >= 20, whole euros, divisible by 10 and not 10 or 30
    bool isValidBillAmount(double amount) const;
    bool hasSufficientFunds(double amount) const;

    // Shows an error label for a short period
    void showWithdrawError(QLabel *label, const QString &text, int ms = 3000);

    void applySaldoTextColors();
    void applyWithdrawConfirmStyle();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private slots:
    void on_btnSaldo_clicked();
    void on_btnTapahtumat_clicked();
    void on_btnNostaRahaa_clicked();
    void on_btnTakaisinSaldo_clicked();
    void on_btnTakaisinTapahtumat_clicked();
    void on_btnLogOut_clicked();
    void on_btnTakaisinLogOut_clicked();
    void on_btnLogOutVahvista_clicked();
    void on_btnTakaisinNostaVahvista_clicked();
    void on_btnNosta20_clicked();
    void on_btnNosta40_clicked();
    void on_btnNosta50_clicked();
    void on_btnNosta100_clicked();
    void on_btnNostaMuu_clicked();
    void on_btnTakaisinNostaValitse_clicked();
    void on_btnTapahtumatVasen_clicked();
    void on_btnTapahtumatOikea_clicked();
    void on_btnNostaVahvistaVahvista_clicked();
};

#endif // ACCOUNT_H
