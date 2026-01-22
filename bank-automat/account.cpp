#include "account.h"
#include "ui_account.h"
#include <qpainter.h>
#include <cmath>
#include <QRegularExpression>

#include <QStandardItemModel>

bool account::isValidBillAmount(double amount) const
{
    // Must be whole euros
    const double rounded = std::round(amount);
    if (std::fabs(amount - rounded) > 1e-9) return false;
    
    const int euros = static_cast<int>(rounded);
    if (euros < 20) return false;

    // With 20€ and 50€ bills: valid amounts are multiples of 10, except 10 and 30
    if (euros % 10 != 0) return false;
    if (euros == 10 || euros == 30) return false;
    
    return true;
}

bool account::hasSufficientFunds(double amount) const
{
    if (cardtype == "debit") return amount <= saldo;
    // credit: allow withdrawing up to remaining credit
    return amount <= (creditlimit - saldo);
}

void account::showWithdrawError(QLabel *label, const QString &text, int ms)
{
    if (!label) return;
    const QString original = label->text();
    label->setText(text);
    label->show();
    QTimer::singleShot(ms, this, [label, original]() {
        label->hide();
        label->setText(original);
    });
}

// Helper-funktio, joka asettaa labelin tekstivärin turvallisesti
// Poistaa ensin mahdollisen aiemmin asetetun color-tyylin, jotta stylesheet ei kasva jokaisella kutsulla
static void setLabelColor(QLabel* lbl, const QString& color)
{
    if (!lbl) return;

    // Regex, jolla poistetaan aiempi "color: ...;" määrittely
    // Static -> luodaan vain kerran, ei joka kutsulla
    static const QRegularExpression colorRe("color\\s*:\\s*[^;]+;");

    QString s = lbl->styleSheet();
    s.remove(colorRe); // poistetaan vanha väri
    s += " color: " + color + ";"; // lisätään uusi väri
    lbl->setStyleSheet(s);
}

// Asettaa saldon näkymän tekstivärit
// Erottaa ulkoasun (boksi) ja logiikan (värit)
// Tätä kutsutaan aina kun saldo-näkymä avataan
void account::applySaldoTextColors()
{
    // Värit valittu niin, että teksti näkyy varmasti valkoisella taustalla
    const QString mainColor = "#000000"; // varsinainen saldo
    const QString infoColor = "#333333"; // credit limit / info
    const QString positive  = "#0A7A0A"; // vihreä: luottoa jäljellä
    const QString warning   = "#B00020"; // punainen: luotto ylitetty

    setLabelColor(ui->labelSaldoSaldo, mainColor);
    setLabelColor(ui->labelSaldoCreditLimit, infoColor);

    // Credit-tilillä luottoa jäljellä -väri riippuu tilanteesta
    QString luottoColor = infoColor;
    if (cardtype == "credit") {
        double creditLeft = creditlimit - saldo;
        luottoColor = (creditLeft < 0) ? warning : positive;
    }
    setLabelColor(ui->labelSaldoLuottoaJaljella, luottoColor);
}

// Asettaa noston vahvistusnäytön summalabelin tyylin (väri tms.)
// Kutsutaan aina ennen siirtymistä screenNostaVahvista -näkymään
void account::applyWithdrawConfirmStyle()
{
    // Varmistetaan että teksti näkyy valkoisella taustalla
    setLabelColor(ui->labelNostaVahvistaSumma, "#000000");
}

account::account(QString cardnumber, QString cardtype,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::account),
    cardnumber(cardnumber),
    cardtype(cardtype)
{
    ui->setupUi(this);
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);

    // Saldokenttien yhteinen ulkoasu
    // Asetetaan vain kerran konstruktorissa, jotta samaa tyyliä ei tarvitse toistaa joka näkymän avauksessa
    const QString saldoBoxStyle =
        "font-size: 18pt;"
        "qproperty-alignment: 'AlignRight';"
        "border: 5px solid #7FABC4;"
        "border-radius: 15px;"
        "background: #FFFFFF;"
        "padding: 6px;";

    // Pakotetaan saldolabelit piirtämään oma tausta
    ui->labelSaldoSaldo->setAutoFillBackground(true);
    ui->labelSaldoCreditLimit->setAutoFillBackground(true);
    ui->labelSaldoLuottoaJaljella->setAutoFillBackground(true);

    ui->labelSaldoSaldo->setStyleSheet(saldoBoxStyle);
    ui->labelSaldoCreditLimit->setStyleSheet(saldoBoxStyle);
    ui->labelSaldoLuottoaJaljella->setStyleSheet(saldoBoxStyle);

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
        "background: white;" // Tausta piirretään stylesheetin kautta
        "color: #000000;" // Musta teksti valkoista taustaa vasten
        "padding: 6px;"
    );
    // Pakottaa Qt:n piirtämään widgetin taustan stylesheetin mukaisesti
    // Tämä on tärkeää, koska ilman tätä QLabel voi jäädä läpinäkyväksi
    ui->labelNostaVahvistaSumma->setAttribute(Qt::WA_StyledBackground, true);

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
    ui->labelSaldoCreditLimit->setText(QString::asprintf("%.2f €", creditlimit));
    ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit-saldo));

    //piilotetaan vain Creditillä käytössä olevat tiedot jos debit
    if (cardtype == "debit"){
        ui->labelSaldoCreditLimit->hide();
        ui->labelSaldoLuottoaJaljella->hide();
        ui->labelSaldoCreditText->hide();
        ui->labelSaldoLuottoText->hide();
    }

    // Asetetaan tekstivärit lopuksi, jotta ne eivät yliajaudu muiden tyylien toimesta
    applySaldoTextColors();
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
    // Asetetaan vahvistusnäkymän tekstivärit ennen näkymän vaihtoa
    applyWithdrawConfirmStyle();
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta40_clicked()
{
    nostosumma = 40;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    applyWithdrawConfirmStyle();
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta50_clicked()
{
    nostosumma = 50;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    applyWithdrawConfirmStyle();
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNosta100_clicked()
{
    nostosumma = 100;
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    applyWithdrawConfirmStyle();
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}


void account::on_btnNostaMuu_clicked()
{
    //tarkistetaan että inputin saa pyöräytettyä doubleksi
    bool ok;
    nostosumma = ui->labelNostosumma->text().toDouble(&ok);
    ui->labelNostosumma->setText("");

    if (!ok || nostosumma <= 0) {
        showWithdrawError(ui->labelNostaValitseVirhe, "Virheellinen summa");
        return;
    }

    // ATM supports only 20€ and 50€ bills
    if (!isValidBillAmount(nostosumma)) {
        showWithdrawError(ui->labelNostaValitseVirhe, "Virheellinen summa (vain 20€ ja 50€)");
        return;
    }

    // Check funds/credit
    if (!hasSufficientFunds(nostosumma)) {
        showWithdrawError(ui->labelNostaValitseVirhe, "Virheellinen summa");
        showWithdrawError(ui->labelNostaValitseKate, "Kate ei riitä");
        return;
    }

    ui->labelNostaValitseVirhe->hide();
    ui->labelNostaValitseKate->hide();
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
    applyWithdrawConfirmStyle();
    ui->stackedAccount->setCurrentWidget(ui->screenNostaVahvista);
}

void account::on_btnTapahtumatVasen_clicked()
{
    tapahtumat->prevPage();
}


void account::on_btnTapahtumatOikea_clicked()
{
    tapahtumat->nextPage();
}

