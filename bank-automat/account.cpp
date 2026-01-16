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
    ui->labelLoginCardnumber->setText(cardnumber + " " + cardtype);
    ui->labelLoginCardnumber->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignCenter';"
        );
    ui->labelNostaVahvistaSumma->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background-color: white;"
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
    ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo));
    ui->labelSaldoSaldo->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background-color: white;"
        );
    ui->labelSaldoCreditLimit->setText(QString::asprintf("%.2f €", creditlimit));
    ui->labelSaldoCreditLimit->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background-color: white;"
        );
    ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit-saldo));
    ui->labelSaldoLuottoaJaljella->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background-color: white;"
        );
    //piilotetaan vain Creditillä käytössä olevat tiedot jos debit
    if (cardtype == "debit"){
        ui->labelSaldoCreditLimit->hide();
        ui->labelSaldoLuottoaJaljella->hide();
        ui->labelSaldoCreditText->hide();
        ui->labelSaldoLuottoText->hide();
    }
}


void account::on_btnTapahtumat_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenTapahtumat);
}


void account::on_btnNostaRahaa_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenNostaValitse);
    //piilotetaan virheraportti aluksi
    ui->labelNostaValitseVirhe->hide();
    ui->labelNostaValitseVirhe->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignCenter';"
        "color: red;"
        );
}


void account::on_btnTakaisinSaldo_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}


void account::on_btnTakaisinTapahtumat_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}

void account::on_btnTakaisinNostaValitse_clicked()
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


void account::on_btnTakaisinNostaVahvista_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenNostaValitse);
}


void account::on_btnNosta20_clicked()
{
    nostosumma = 20;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta40_clicked()
{
    nostosumma = 40;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta50_clicked()
{
    nostosumma = 50;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta100_clicked()
{
    nostosumma = 100;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNostaMuu_clicked()
{
    //tarkistetaan että inputin saa pyöräytettyä doubleksi
    bool ok;
    nostosumma =  ui->labelNostosumma->text().toDouble(&ok);
    ui->labelNostosumma->setText("");
    //joko onnistuu ja eteenpäin tai virheraportti näkyväksi
    if (ok == false or nostosumma <= 0) {
        ui->labelNostaValitseVirhe->show();
    }
    else {
        ui->labelNostaValitseVirhe->hide();
        ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
        ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
    }
}







