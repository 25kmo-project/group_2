#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QWidget>
//Tarvitaan hard resettiin:
#include <QProcess>
#include <QCoreApplication>

namespace Ui {
class account;
}

class account : public QWidget
{
    Q_OBJECT

public:
    explicit account(QString cardnumber, QString cardtype, QWidget *parent = nullptr);
    ~account();

private:
    Ui::account *ui;
    QString cardnumber;
    QString cardtype;
    //annetaan testausta varten jotkut arvot
    double saldo = 355;
    double creditlimit = 1000;
    double nostosumma;

protected:
    void paintEvent(QPaintEvent *event) override;
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
};

#endif // ACCOUNT_H
