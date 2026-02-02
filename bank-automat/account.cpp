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

    // React to successful balance retrieval from the backend
    connect(m_api, &ApiClient::balanceReceived, this,
        [this](int idAccount, double balance, double creditLimit)
        {
            // Ignore events for other accounts
            if (idAccount != m_idAccount) return;
            
            // Update internal state
            saldo = balance;
            creditlimit = creditLimit;
            
            // Update UI only if the balance screen is currently visible
            if (ui->stackedAccount->currentWidget() == ui->screenSaldo) {
                ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo).replace('.',','));
                ui->labelSaldoCreditLimit->setText(QString::asprintf("%.2f €", creditlimit).replace('.',','));
                ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit - saldo).replace('.',','));
            }
        }
    );
    
    // React to a successful withdrawal (debit or credit)
    connect(m_api, &ApiClient::withdrawSucceeded, this,
        [this](int idAccount, double newBalance)
        {
            // Ignore events for other accounts
            if (idAccount != m_idAccount) return;
            // Update local balance
            saldo = newBalance;
            
            // Update UI only if the balance screen is currently visible
            if (ui->stackedAccount->currentWidget() == ui->screenSaldo) {
                ui->labelSaldoSaldo->setText(QString::asprintf("%.2f €", saldo).replace('.',','));
                ui->labelSaldoLuottoaJaljella->setText(QString::asprintf("%.2f €", creditlimit - saldo).replace('.',','));
            }
        }
    );
    
    // React to successful retrieval of account transaction logs
    connect(m_api, &ApiClient::logsReceived, this,
        [this](int idAccount, const QVector<LogItemDto>& logs)
        {
            // Ignore events for other accounts
            if (idAccount != m_idAccount) return;
            
            // Convert log DTOs into a JSON array
            QJsonArray arr;
            for (const auto& item : logs) {
                QJsonObject o;
                o["idLog"] = item.idLog;
                o["time"] = item.time;
                o["balanceChange"] = item.balanceChange;
                arr.append(o);
            }
            // Serialize JSON to bytes and pass it to the log view component
            const QByteArray jsonBytes = QJsonDocument(arr).toJson(QJsonDocument::Compact);
            tapahtumat->setLog(jsonBytes);
        }
    );
    
    // React to any API error (network, validation, backend failure)
    connect(m_api, &ApiClient::requestFailed, this,
        [this](const ApiError& err)
        {
            // Show a withdrawal-related error message in the UI
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


// Called when the user clicks the "Confirm Withdrawal" button
void account::on_btnNostaVahvistaVahvista_clicked()
{
    // Amount to withdraw (already validated earlier)
    const double amount = nostosumma;
    // Determine whether this is a credit account
    // Credit accounts have a positive credit limit
    const bool isCredit = creditlimit > 0.0;
    
    // Call the appropriate API endpoint based on account type
    if (isCredit) {
        // Credit withdrawal
        m_api->withdrawCredit(m_idAccount, amount);
    } else {
        // Debit withdrawal
        m_api->withdrawDebit(m_idAccount, amount);
    }
    
    // After sending the request, switch back to the login / idle screen
    // (actual balance update happens asynchronously via signals)
    ui->stackedAccount->setCurrentWidget(ui->screenLogin);
}

// Handle keyboard input globally for this dialog
void account::keyPressEvent(QKeyEvent *event)
{
    // Check if the current screen is the withdrawal selection screen
    const bool onWithdrawSelect =
    (ui->stackedAccount->currentWidget() == ui->screenNostaValitse);
    
    // Handle Enter / Return key specifically on the withdraw selection screen
    if (onWithdrawSelect &&
        (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
        {
            // If the amount input does not have focus, move focus to it
            if (!ui->labelNostosumma->hasFocus()) {
                ui->labelNostosumma->setFocus();
                event->accept();
                return;
            }
            
            // If the amount field has text, treat Enter as "confirm custom amount"
            if (!ui->labelNostosumma->text().trimmed().isEmpty()) {
                on_btnNostaMuu_clicked();
                event->accept();
                return;
            }
            
            // Consume the event even if nothing else happens
            event->accept();
            return;
        }
        
        // For all other keys and screens, use default dialog behavior
        QDialog::keyPressEvent(event);
    }
