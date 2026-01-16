#include "account.h"
#include "ui_account.h"
#include <qpainter.h>


account::account(QString cardnumber, QString cardtype,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::account),
    cardnumber(cardnumber),
    cardtype(cardtype)
{
    ui->setupUi(this);
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
    ui->labelLoginCardnumber->setText(cardnumber);
    ui->labelLoginCardnumber->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignCenter';"
        );
}

account::~account()
{
    delete ui;
}

//valintaruudun tausta, sama kuin edellisissä
void account::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    static const QPixmap accountPix(":/images/backgroundLogin.png");
    painter.drawPixmap(rect(), accountPix);
}

void account::on_btnSaldo_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenSaldo);
}


void account::on_btnTapahtumat_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenTapahtumat);
}


void account::on_btnNostaRahaa_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenNosta);
}


void account::on_btnTakaisinSaldo_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}


void account::on_btnTakaisinTapahtumat_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}


void account::on_btnTakaisinNosta_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}


void account::on_btnLogOut_clicked()
{
     ui->stackedAccount->setCurrentWidget(ui->screenLogOut);
}


void account::on_btnTakaisinLogOut_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}


void account::on_btnLogOutVahvista_clicked()
{
    //Hard reset
    //StartDetached = kokonaan irtonainen, uusi sovellus
    //applicationFilePath = Tämän sovelluksen sijainnista
    //{} = startDetached function vaatii argumentit että ajaa oikein, annetaan tyhjä
    QProcess::startDetached(QCoreApplication::applicationFilePath(), {});
    qApp->quit();   //sulkee tämän version sovelluksesta
}

