#include "account.h"
#include "ui_account.h"
#include <qpainter.h>
#include <cmath>
#include <QRegularExpression>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

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
    const bool isCredit = creditlimit > 0.0;
    if (!isCredit) return amount <= saldo;    
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

account::account(int idAccount, const QString& idUser, const QString& fName, ApiClient* api, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::account)
    , m_api(api)
    , m_idAccount(idAccount)
    , m_idUser(idUser)
    , m_fName(fName)
{
    ui->setupUi(this);
    m_api->getBalance(m_idAccount);
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);

    ui->labelLoginCardnumber->setText("Käyttäjä: " + m_fName);

    tapahtumat = new logs(this);
    ui->tableTapahtumat->setModel(tapahtumat->getModel());

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

    connect(m_api, &ApiClient::balanceReceived, this,
        [this](int idAccount, double balance, double creditLimit)
        {
            if (idAccount != m_idAccount) return;
            
            saldo = balance;
            creditlimit = creditLimit;
            
            if (ui->stackedAccount->currentWidget() == ui->screenSaldo) {
                ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo));
                ui->labelSaldoCreditLimit->setText(QString::asprintf("%.2f €", creditlimit));
                ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit - saldo));
            }
        }
    );
    
    connect(m_api, &ApiClient::withdrawSucceeded, this,
        [this](int idAccount, double newBalance)
        {
            if (idAccount != m_idAccount) return;
            saldo = newBalance;
            
            if (ui->stackedAccount->currentWidget() == ui->screenSaldo) {
                ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo));
                ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit - saldo));
            }
        }
    );
    
    connect(m_api, &ApiClient::logsReceived, this,
        [this](int idAccount, const QVector<LogItemDto>& logs)
        {
            if (idAccount != m_idAccount) return;
            
            QJsonArray arr;
            for (const auto& item : logs) {
                QJsonObject o;
                o["idlog"] = item.idLog;
                o["time"] = item.time;
                o["balancechange"] = item.balanceChange;
                arr.append(o);
            }
            const QByteArray jsonBytes = QJsonDocument(arr).toJson(QJsonDocument::Compact);
            tapahtumat->setLog(jsonBytes);
        }
    );
    
    connect(m_api, &ApiClient::requestFailed, this,
        [this](const ApiError& err)
        {
            showWithdrawError(
                ui->labelNostaValitseVirhe_2,
                err.message.isEmpty() ? "Tapahtui virhe" : err.message
            );
        }
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
    m_api->getBalance(m_idAccount);

    ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo));
    ui->labelSaldoCreditLimit->setText(QString::asprintf("%.2f €", creditlimit));
    ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit-saldo));

    //piilotetaan vain Creditillä käytössä olevat tiedot jos debit
    const bool isDebit = (creditlimit <= 0.0);
    
    if (isDebit) {
        ui->labelSaldoCreditLimit->hide();
        ui->labelSaldoLuottoaJaljella->hide();
        ui->labelSaldoCreditText->hide();
        ui->labelSaldoLuottoText->hide();
    } else {
        ui->labelSaldoCreditLimit->show();
        ui->labelSaldoLuottoaJaljella->show();
        ui->labelSaldoCreditText->show();
        ui->labelSaldoLuottoText->show();
    }
}


void account::on_btnTapahtumat_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenTapahtumat);
    m_api->getAccountLogs(m_idAccount);
}


void account::on_btnNostaRahaa_clicked()
{
    ui->stackedAccount->setCurrentWidget(ui->screenNostaValitse);
    //piilotetaan virheraporttit aluksi
    ui->labelNostaValitseVirhe->hide();
    ui->labelNostaValitseKate->hide();
    ui->labelNostaValitseVirhe_2->hide();
    ui->labelNostosumma->clear();

    //kursori summakenttaään
    ui->labelNostosumma->setFocus();
    //mahdollisuus entteriä painamalla valita muu summa
    connect(ui->labelNostosumma, &QLineEdit::returnPressed, this, &account::on_btnNostaMuu_clicked, Qt::UniqueConnection);
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
    nostosumma = ui->labelNostosumma->text().toDouble(&ok);
    ui->labelNostosumma->setText("");

    if (!ok || nostosumma <= 0) {
        showWithdrawError(ui->labelNostaValitseVirhe_2, "Virheellinen summa");
        return;
    }

    // ATM supports only 20€ and 50€ bills
    if (!isValidBillAmount(nostosumma)) {
        showWithdrawError(ui->labelNostaValitseVirhe, "Virheellinen summa (vain 20€ ja 50€)");
        return;
    }

    // Check funds/credit
    if (!hasSufficientFunds(nostosumma)) {
        showWithdrawError(ui->labelNostaValitseKate, "Kate ei riitä");
        return;
    }

    ui->labelNostaValitseVirhe->hide();
    ui->labelNostaValitseKate->hide();
    ui->labelNostaVahvistaSumma->setText(QString::asprintf("%.2f €", nostosumma));
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


void account::on_btnNostaVahvistaVahvista_clicked()
{
    const double amount = nostosumma;
    const bool isCredit = creditlimit > 0.0;
    
    if (isCredit) {
        m_api->withdrawCredit(m_idAccount, amount);
    } else {
        m_api->withdrawDebit(m_idAccount, amount);
    }
    
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}

void account::keyPressEvent(QKeyEvent *event)
{
    const bool onWithdrawSelect =
    (ui->stackedAccount->currentWidget() == ui->screenNostaValitse);
    
    if (onWithdrawSelect &&
        (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
        {
            if (!ui->labelNostosumma->hasFocus()) {
                ui->labelNostosumma->setFocus();
                event->accept();
                return;
            }
            
            if (!ui->labelNostosumma->text().trimmed().isEmpty()) {
                on_btnNostaMuu_clicked();
                event->accept();
                return;
            }
            
            event->accept();
            return;
        }
        
        QDialog::keyPressEvent(event);
    }
