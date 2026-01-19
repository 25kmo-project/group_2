#include "account.h"
#include "ui_account.h"
#include <qpainter.h>

#include <QStandardItemModel>


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
    //Monta tapaa päästä allaolevaan, niin formatoidaan täällä kerran eikä joka paikassa erikseen
    ui->labelNostaVahvistaSumma->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background-color: white;"
        );

    //testidataa taulua varten
    testData = R"([
    {"idlog": 1, "time": "2026-01-18 12:00", "balancechange": -20.50},
    {"idlog": 2, "time": "2026-01-18 12:05", "balancechange": 100.00},
    {"idlog": 3, "time": "2026-01-18 13:30", "balancechange": -5.00},
    {"idlog": 4, "time": "2026-01-18 14:15", "balancechange": -12.80},
    {"idlog": 5, "time": "2026-01-18 15:00", "balancechange": 2450.00},
    {"idlog": 6, "time": "2026-01-18 16:45", "balancechange": -65.20},
    {"idlog": 7, "time": "2026-01-19 08:30", "balancechange": -4.50},
    {"idlog": 8, "time": "2026-01-19 09:12", "balancechange": -110.00},
    {"idlog": 9, "time": "2026-01-19 11:50", "balancechange": 15.00},
    {"idlog": 10, "time": "2026-01-19 13:20", "balancechange": -22.15},
    {"idlog": 11, "time": "2026-01-19 17:05", "balancechange": -45.00},
    {"idlog": 12, "time": "2026-01-20 10:00", "balancechange": -3.20},
    {"idlog": 13, "time": "2026-01-20 12:30", "balancechange": 50.00},
    {"idlog": 14, "time": "2026-01-20 15:45", "balancechange": -200.00},
    {"idlog": 15, "time": "2026-01-20 18:20", "balancechange": -8.90}
    ])";

    tapahtumat = new logs(this);
    ui->tableTapahtumat->setModel(tapahtumat->getModel());
    tapahtumat->setLog(testData);

    //Estetään käyttäjää  muokkaamasta tablen ulkonäköä ja säädetään ulkonäköä

    ui->tableTapahtumat->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableTapahtumat->setColumnWidth(0, 50);
    ui->tableTapahtumat->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableTapahtumat->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableTapahtumat->horizontalHeader()->setSectionsMovable(false);
    ui->tableTapahtumat->horizontalHeader()->setSectionsClickable(false);
    ui->tableTapahtumat->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableTapahtumat->horizontalHeader()->setFixedHeight(40);
    ui->tableTapahtumat->verticalHeader()->setVisible(false);
    ui->tableTapahtumat->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

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
    //piilotetaan virheraporttit aluksi
    ui->labelNostaValitseVirhe->hide();
    ui->labelNostaValitseKate->hide();
    ui->labelNostaValitseVirhe->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignCenter';"
        "color: red;"
        );
    ui->labelNostaValitseKate->setStyleSheet(
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignCenter';"
        "color: red;"
        );

    //kursori summakenttaään
    ui->labelNostosumma->setFocus();
    //mahdollisuus entteriä painamalla valita muu summa
    connect(ui->labelNostosumma, &QLineEdit::returnPressed, this, &account::on_btnNostaMuu_clicked);
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
        QTimer::singleShot(2000, this, [this]() {
            ui->labelNostaValitseVirhe->hide(); // Piilottaa tekstin 2s päästä
        });
    }
    else if ((cardtype == "debit" and nostosumma > saldo) or (cardtype == "credit" and nostosumma > (creditlimit-saldo))) {
        ui->labelNostaValitseVirhe->show();
        ui->labelNostaValitseKate->show();
        QTimer::singleShot(2000, this, [this]() {
            ui->labelNostaValitseVirhe->hide(); // Piilottaa tekstin 2s päästä
            ui->labelNostaValitseKate->hide();
        });
    }
    else {
        ui->labelNostaValitseVirhe->hide();
        ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
        ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
    }
}

void account::on_btnTapahtumatVasen_clicked()
{
    tapahtumat->prevPage();
}


void account::on_btnTapahtumatOikea_clicked()
{
    tapahtumat->nextPage();
}

