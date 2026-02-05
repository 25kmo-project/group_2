#include "adminwindow.h"
#include "accountsdata.h"
#include "apiclient.h"
#include "carddata.h"
#include "logs.h"
#include "ui_adminwindow.h"
#include "userdata.h"
#include <QProcess>
#include <qpainter.h>

adminwindow::adminwindow(QString idUser, ApiClient *api, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminwindow),
    m_idUser(idUser),
    m_api(api)
{
    ui->setupUi(this);
    ui->labelAdminUser->setText("Adminkäyttäjä: "+m_idUser);
    ui->stackedAdmin->setCurrentWidget(ui->screenAsiakkaat);
    m_api->getAllUsers();

    connect(ui->lineLokitTiliID, &QLineEdit::returnPressed, this, &adminwindow::on_btnLokitHae_clicked);

    userData = new userdata(this);
    ui->tableUserData->setModel(userData->getModel());
    userData->setUserData(testUserData);

    accountsData = new accountsdata(this);
    ui->tableAccountsData->setModel(accountsData->getModel());
    accountsData->setAccountsData(testAccountsData);

    cardData = new carddata(m_api,this);
    ui->tableCardData->setModel(cardData->getModel());
    cardData->setCardData(testCardData);

    logData = new logs(this);
    ui->tableLokit->setModel(logData->getModelAdmin());
    logData->setLog(testLogData);

    ui->tableUserData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(0, 80);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(1, 80);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(2, 80);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(4, 60);

    //IdAccount only accept integers
    ui->lineTilitIdaccount->setValidator(new QIntValidator(0, 9999999, this));
    //Balance and creditLimit only accept doubles of correct kind
    ui->lineTilitBalance->setValidator(new QDoubleValidator(0, 9999999999, 2, this));
    ui->lineTilitCreditlimit->setValidator(new QDoubleValidator(0, 9999999999, 2, this));
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableAccountsData->setColumnWidth(0, 80);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableAccountsData->setColumnWidth(1, 80);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    ui->tableCardData->verticalHeader()->setVisible(false);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(2, 75);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(3, 50);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(4, 75);

    //IdAccount only accept integers
    ui->lineLokitTiliID->setValidator(new QIntValidator(0, 9999999, this));
    ui->tableLokit->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableLokit->setColumnWidth(0, 50);
    ui->tableLokit->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableLokit->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    connect(m_api, &ApiClient::userReceived, this, [this](QByteArray userInfo) {
        userData->setUserData(userInfo);
        userData->updateModel();
        //clears inputs
        this->clearAsiakkaatInputs();
    });

    connect(m_api, &ApiClient::allUsersReceived, this, [this](QByteArray allUsersInfo) {
        userData->setUserData(allUsersInfo);
        userData->updateModel();
        //clears inputs
        this->clearAsiakkaatInputs();
    });

    connect(m_api, &ApiClient::userCreated, this, [this](QString id) {
        //Just in case something goes wrong it doesnt crash with empty id
        if (!id.isEmpty()) {
            m_api->getUser(id);
            this->clearAsiakkaatInputs();
        }
    });

    connect(m_api, &ApiClient::userUpdated, this, [this](QString id) {
        //Just in case something goes wrong it doesnt crash with empty id
        if (!id.isEmpty()) {
            m_api->getUser(id);
            //clears inputs
            this->clearAsiakkaatInputs();
        }
    });

    connect(m_api, &ApiClient::userDeleted, this, [this]() {
        ui->lineAsiakkaatID->clear();
        //Clears the table after deleting
        userData->setUserData(QByteArray());
        this->clearAsiakkaatInputs();
    });

    connect(m_api, &ApiClient::accountReceived, this, [this](QByteArray accountInfo) {
        accountsData->setAccountsData(accountInfo);
        accountsData->updateModel();
        //clears inputs
        this->clearTilitInputs();
    });

    connect(m_api, &ApiClient::allAccountsReceived, this, [this](QByteArray allAccountsInfo) {
        accountsData->setAccountsData(allAccountsInfo);
        accountsData->updateModel();
        //clears inputs
        this->clearTilitInputs();
    });

    connect(m_api, &ApiClient::accountCreated, this, [this](QByteArray accountInfo) {
        accountsData->setAccountsData(accountInfo);
        accountsData->updateModel();
        //clears inputs
        this->clearTilitInputs();
    });

    connect(m_api, &ApiClient::accountCreditUpdated, this, [this](int idAccount) {
        m_api->getAccount(idAccount);
        //clears inputs
        this->clearTilitInputs();
    });

    connect(m_api, &ApiClient::accountDeleted, this, [this] {
        //Clears the table after deleting
        accountsData->setAccountsData(QByteArray());
        //clears inputs
        this->clearTilitInputs();
    });

    connect(m_api, &ApiClient::allCardsReceived, this, [this](QByteArray allCards) {
        cardData->setCardData(allCards);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardReceived, this, [this](QByteArray card) {
        cardData->setCardData(card);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardCreated, this, [this](QByteArray card) {
        cardData->setCardData(card);
        // waitingAccountID > 0 only if something actually saved. Initialized to 0 and reset after succesful operation
        if (this->waitingAccountId > 0) {
            QJsonDocument doc = QJsonDocument::fromJson(card);
            QString idCard = doc.object().value("idCard").toString();
            m_api->linkCard(idCard, this->waitingAccountId);
            this->waitingAccountId = 0;
        }
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardDeleted, this, [this] {
        m_api->getAllCards();
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::PINUpdated, this, [this](QString idCard) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardAccountLinked, this, [this](QByteArray cardAccount) {
        QJsonDocument doc = QJsonDocument::fromJson(cardAccount);
        QString idCard = doc.object().value("idCard").toString();
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardLocked, this, [this](QString idCard) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardUnlocked, this, [this](QString idCard) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardLinkDeleted, this, [this](QString idCard) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::cardLinkUpdated, this, [this](QString idCard) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    });

    connect(m_api, &ApiClient::adminLogsReceived, this, [this](QByteArray adminLogs) {
        logData->setLog(adminLogs);
        //clears input
        ui->lineLokitTiliID->clear();
    });

    // Shows card type error from backend
    connect(m_api, &ApiClient::requestFailed, this, [this](ApiError error) {
        if (error.message == "Cannot change card between credit and debit accounts") {
            ui->labelKortitError->setText("Cannot change card\nbetween credit and\ndebit accounts");
        }
    });
}

adminwindow::~adminwindow()
{
    delete ui;
}

void adminwindow::clearAsiakkaatInputs()
{
    ui->lineAsiakkaatID->clear();
    ui->lineAsiakkaatFname->clear();
    ui->lineAsiakkaatLname->clear();
    ui->lineAsiakkaatAddress->clear();
    ui->lineAsiakkaatRole->clear();

}

void adminwindow::clearTilitInputs()
{
    ui->lineTilitIdaccount->clear();
    ui->lineTilitIduser->clear();
    ui->lineTilitBalance->clear();
    ui->lineTilitCreditlimit->clear();
}

void adminwindow::clearKortitInputs()
{
    ui->lineKortitIdCard->clear();
    ui->lineKortitLukossa->clear();
    ui->lineKortitPinYritykset->clear();
    ui->lineKortitIdUser->clear();
    ui->lineKortitPIN->clear();
    ui->lineKortitTilit->clear();
    ui->lineKortitUusiIdAccount->clear();
    ui->labelKortitError->setText("");
}

//adminruudun tausta, jälleen sama
void adminwindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    static const QPixmap selectPix(":/images/backgroundLogin.png");
    painter.drawPixmap(rect(), selectPix);
}

void adminwindow::on_btnAsiakkaatLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenAsiakkaat);
    m_api->getAllUsers();
}

void adminwindow::on_btnTilitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenTilit);
    m_api->getAllAccounts();
}

void adminwindow::on_btnKortitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenKortit);
    m_api->getAllCards();
    this->clearKortitInputs();
}

void adminwindow::on_btnLokitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenLokit);
    ui->lineLokitTiliID->setFocus();
}

void adminwindow::on_btnLogOutLowBar_2_clicked()
{
    //Hard reset
    //StartDetached = kokonaan irtonainen, uusi sovellus
    //applicationFilePath = Tämän sovelluksen sijainnista
    //{} = startDetached function vaatii argumentit että ajaa oikein, annetaan tyhjä
    QProcess::startDetached(QCoreApplication::applicationFilePath(), {});
    qApp->quit();   //sulkee tämän version sovelluksesta
}

void adminwindow::on_btnKayttajaLuoUusi_clicked()
{
    QString idUser = ui->lineAsiakkaatID->text().trimmed();
    QString fname = ui->lineAsiakkaatFname->text().trimmed();
    QString lname = ui->lineAsiakkaatLname->text().trimmed();
    QString address = ui->lineAsiakkaatAddress->text().trimmed();
    QString role = ui->lineAsiakkaatRole->text().trimmed();
    if (!idUser.isEmpty()) {
        m_api->addUser(idUser, fname, lname, address, role);
    }
}


void adminwindow::on_btnLokitHae_clicked()
{
    QString idAccount = ui->lineLokitTiliID->text().trimmed();
    if (!idAccount.isEmpty()) {
        bool ok;
        int intIdAccount = idAccount.toInt(&ok);
        if (ok) {
            m_api->getAdminLogs(intIdAccount);
        }
    }
}


void adminwindow::on_btnKayttajaHae_clicked()
{
    QString idUser = ui->lineAsiakkaatID->text();
    if (!idUser.isEmpty()) {
        m_api->getUser(idUser);
    }
}


void adminwindow::on_btnKayttajaPaivitaTietoja_clicked()
{
    //If some inputs are empty, old data is pulled from table so that it doesnt get emptied by db procedure
    QString idUser = ui->lineAsiakkaatID->text().trimmed();
    QString inputFName = ui->lineAsiakkaatFname->text().trimmed();
    QString inputLName = ui->lineAsiakkaatLname->text().trimmed();
    QString inputAddress = ui->lineAsiakkaatAddress->text().trimmed();
    //Cant change this currently
    //QString inputRole = ui->lineAsiakkaatRole->text().trimmed();

    QString currentFName = ui->tableUserData->model()->index(0,1).data().toString();
    QString currentLName = ui->tableUserData->model()->index(0,2).data().toString();
    QString currentAddress = ui->tableUserData->model()->index(0,3).data().toString();
    //Cant change role currently
    //QString currentRole = ui->tableUserData->model()->index(0,4).data().toString();
    if (!idUser.isEmpty()) {
        QString fname = inputFName.isEmpty() ? currentFName : inputFName;
        QString lname = inputLName.isEmpty() ? currentLName : inputLName;
        QString streetAddress = inputAddress.isEmpty() ? currentAddress : inputAddress;
        //QString role = inputRole.isEmpty() ? currentRole : inputRole;
        m_api->updateUser(idUser, fname, lname, streetAddress);
    }
}


void adminwindow::on_btnKayttajaPoista_clicked()
{
    QString idUser = ui->lineAsiakkaatID->text();
    if (!idUser.isEmpty()) {
        m_api->deleteUser(idUser);
    }
}


void adminwindow::on_btnTiliHae_clicked()
{
    //Remove spaces and check that input is given and it is a number
    QString idAccount = ui->lineTilitIdaccount->text().trimmed();
    if (!idAccount.isEmpty()) {
        bool ok;
        int intIdAccount = idAccount.toInt(&ok);
        if (ok) {
            m_api->getAccount(intIdAccount);
        }
    }
}


void adminwindow::on_btnTiliLuoUusi_clicked()
{
    QString idUser = ui->lineTilitIduser->text().trimmed();
    double balance = ui->lineTilitBalance->text().trimmed().toDouble();
    double creditLimit = ui->lineTilitCreditlimit->text().trimmed().toDouble();
    if (!idUser.isEmpty()) {
        m_api->addAccount(idUser, balance, creditLimit);
    }
}


void adminwindow::on_btnTiliPaivitaLuottoraja_clicked()
{
    QString idAccount = ui->lineTilitIdaccount->text().trimmed();
    QString creditLimit = ui->lineTilitCreditlimit->text().trimmed().replace(',','.');
    if (!idAccount.isEmpty() && !creditLimit.isEmpty()) {
        bool okId;
        bool okCredit;
        int intIdAccount = idAccount.toInt(&okId);
        double doubleCreditLimit = creditLimit.toDouble(&okCredit);
        if (okId && okCredit) {
            m_api->updateCreditLimit(intIdAccount, doubleCreditLimit);
        }
    }
}



void adminwindow::on_btnTiliPoistaTili_clicked()
{
    QString idAccount = ui->lineTilitIdaccount->text().trimmed();
    if (!idAccount.isEmpty()) {
        bool ok;
        int intIdAccount = idAccount.toInt(&ok);
        if (ok) {
            m_api->deleteAccount(intIdAccount);
        }
    }
}


void adminwindow::on_btnKayttajaHaeKaikki_clicked()
{
    m_api->getAllUsers();
}


void adminwindow::on_btnTiliHaeKaikki_clicked()
{
    m_api->getAllAccounts();
}


void adminwindow::on_btnKorttiLuoUusi_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    QString idUser = ui->lineKortitIdUser->text().trimmed();
    QString PIN = ui->lineKortitPIN->text().trimmed();
    QString idAccount = ui->lineKortitTilit->text().trimmed();
    bool ok;
    int intIdAccount = idAccount.toInt(&ok);
    if (!idCard.isEmpty() && !idUser.isEmpty() && !PIN.isEmpty() && ok) {
        this->waitingAccountId = intIdAccount;
        m_api->addCard(idCard, idUser, PIN);
        this->clearKortitInputs();
    }
    else {
        ui->labelKortitError->setText("idCard, idUser,\nidAccount tai PIN\npuuttuu");
    }
}


void adminwindow::on_btnKorttiHaeKaikki_clicked()
{
    m_api->getAllCards();
    this->clearKortitInputs();
}


void adminwindow::on_btnKorttiHaeKortti_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    if (!idCard.isEmpty()) {
        m_api->getCard(idCard);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiPoista_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    if (!idCard.isEmpty()) {
        m_api->deleteCard(idCard);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiPaivitaPIN_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    QString PIN = ui->lineKortitPIN->text().trimmed();
    if (!idCard.isEmpty() && !PIN.isEmpty()) {
        m_api->updatePIN(idCard, PIN);
        this->clearKortitInputs();
    }
    else {
        ui->labelKortitError->setText("idCard tai PIN\npuuttuu");
    }
}


void adminwindow::on_btnKorttiLukitse_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    if (!idCard.isEmpty()) {
        m_api->lockCard(idCard);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiAvaa_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    if (!idCard.isEmpty()) {
        m_api->unlockCard(idCard);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiLinkita_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    QString idAccount = ui->lineKortitTilit->text().trimmed();
    bool ok;
    int intIdAccount = idAccount.toInt(&ok);
    if (!idCard.isEmpty() && !idAccount.isEmpty() && ok) {
        m_api->linkCard(idCard, intIdAccount);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiPoistaLinkitys_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    QString idAccount = ui->lineKortitTilit->text().trimmed();
    bool ok;
    int intIdAccount = idAccount.toInt(&ok);
    if (!idCard.isEmpty() && !idAccount.isEmpty() && ok) {
        m_api->deleteLink(idCard, intIdAccount);
        this->clearKortitInputs();
    }
}


void adminwindow::on_btnKorttiPaivitaLinkitys_clicked()
{
    QString idCard = ui->lineKortitIdCard->text().trimmed();
    QString idAccount = ui->lineKortitTilit->text().trimmed();
    QString newIdAccount = ui->lineKortitUusiIdAccount->text().trimmed();
    bool ok1;
    bool ok2;
    int intIdAccount = idAccount.toInt(&ok1);
    int intNewIdAccount = newIdAccount.toInt(&ok2);
    if (idCard.isEmpty() || idAccount.isEmpty() || newIdAccount.isEmpty()) {
        ui->labelKortitError->setText("idCard, idAccount\ntai uusiIdAccount\npuuttuu");
        return;
    }
    if (!ok1 || !ok2) {
        ui->labelKortitError->setText("idAccount ja\nuusiIdAccount\ei kelpaa");
        return;
    }
    m_api->updateLink(idCard, intIdAccount, intNewIdAccount);
    ui->labelKortitError->setText("");
}

