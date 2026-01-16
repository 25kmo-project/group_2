#include "accountselect.h"
#include "account.h"
#include "ui_accountselect.h"
#include <QPainter>
#include <QPixmap>


accountselect::accountselect(QString message, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::accountselect),
    cardnumber(message)
{
    ui->setupUi(this);
    ui->labelTest->setText(cardnumber);
}

accountselect::~accountselect()
{
    delete ui;
}

void accountselect::on_btnSelectCredit_clicked()
{
    cardtype = "credit";
    openAccountWindow();
}

void accountselect::on_btnSelectDebit_clicked()
{
    cardtype = "debit";
    openAccountWindow();
}

void accountselect::openAccountWindow()
{
    accountWindow = new account(cardnumber, cardtype, nullptr);
    //nulpptr niin voidaan accountselectwindow sammuttaa ja account säilyy
    accountWindow->setAttribute(Qt::WA_DeleteOnClose);
    //Lisätään että muisti vapautetaan oikein suljettaessa, koska nullptr käytössä
    accountWindow->showMaximized();
    this->close();      //suljetaan accountSelectWindow
}

//valintaruudun tausta, sama kuin kirjautumissivulla
void accountselect::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    static const QPixmap selectPix(":/images/backgroundLogin.png");
    painter.drawPixmap(rect(), selectPix);
}



