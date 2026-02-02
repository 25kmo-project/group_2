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

    connect(ui->labelTiliID, &QLineEdit::returnPressed, this, &adminwindow::on_btnLokitHae_clicked);

    //testidataa
    testUserData = R"([
    {"idUser": "1", "firstName": "Aku", "lastName": "Ankka", "streetAddress": "Paratiisitie 13", "role": "user"},
    {"idUser": "2", "firstName": "Roope", "lastName": "Ankka", "streetAddress": "Rahasäiliö 1", "role": "admin"},
    {"idUser": "3", "firstName": "Mikki", "lastName": "Hiiri", "streetAddress": "Jokikatu 43", "role": "user"},
    {"idUser": "4", "firstName": "Hessu", "lastName": "Hopo", "streetAddress": "Koivukatu 7", "role": "user"}
    ])";

    testAccountsData = R"([
    {"idAccount": 1, "idUser": "1", "balance": 500, "crediLimit": 0},
    {"idAccount": 2, "idUser": "2", "balance": 1000000, "creditLimit": 0},
    {"idAccount": 3, "idUser": "2", "balance": 10000, "creditLimit": 50000},
    {"idAccount": 4, "idUser": "3", "balance": 25000, "creditLimit": 0},
    {"idAccount": 5, "idUser": "3", "balance": 250, "creditLimit": 1500},
    {"idAccount": 6, "idUser": "4", "balance": 1000, "creditLimit": 0}
    ])";

    testCardData = R"([
    {"idCard": "1", "cardPIN": "$2b$10$nS6H0mXIsmR8S.r6Yp89neMv5.7.8JmQ6mGzO8Z6V6H8N6f6Y6e6q", "idUser": "1", "isLocked": false},
    {"idCard": "2", "cardPIN": "$2b$10$Eixza975pS8En6Z6v6f6uef6u6e6u6e6u6e6u6e6u6e6u6e6u6e6u", "idUser": "2", "isLocked": false},
    {"idCard": "3", "cardPIN": "$2b$10$K7L9M0N1O2P3Q4R5S6T7U8V9W0X1Y2Z3A4B5C6D7E8F9G0H1I2J3K", "idUser": "3", "isLocked": false},
    {"idCard": "4", "cardPIN": "$2b$10$K7L9M0N1O2P3Q4R5S6f6uef6u6e6u6e6A4BzO8Z6V6H8G0H1I.7.8", "idUser": "4", "isLocked": true}
    ])";

    testLogData = R"([
    {"idLog": 1, "time": "2026-01-18 12:00", "balanceChange": -20.50},
    {"idLog": 2, "time": "2026-01-18 12:05", "balanceChange": 100.00},
    {"idLog": 3, "time": "2026-01-18 13:30", "balanceChange": -5.00},
    {"idLog": 4, "time": "2026-01-18 14:15", "balanceChange": -12.80},
    {"idLog": 5, "time": "2026-01-18 15:00", "balanceChange": 2450.00},
    {"idLog": 6, "time": "2026-01-18 16:45", "balanceChange": -65.20},
    {"idLog": 7, "time": "2026-01-19 08:30", "balanceChange": -4.50},
    {"idLog": 8, "time": "2026-01-19 09:12", "balanceChange": -110.00},
    {"idLog": 9, "time": "2026-01-19 11:50", "balanceChange": 15.00},
    {"idLog": 10, "time": "2026-01-19 13:20", "balanceChange": -22.15},
    {"idLog": 11, "time": "2026-01-19 17:05", "balanceChange": -45.00},
    {"idLog": 12, "time": "2026-01-20 10:00", "balanceChange": -3.20},
    {"idLog": 13, "time": "2026-01-20 12:30", "balanceChange": 50.00},
    {"idLog": 14, "time": "2026-01-20 15:45", "balanceChange": -200.00},
    {"idLog": 15, "time": "2026-01-20 18:20", "balanceChange": -8.90},
    {"idLog": 16, "time": "2026-01-21 09:15", "balanceChange": -35.00},
    {"idLog": 17, "time": "2026-01-21 11:40", "balanceChange": 120.50},
    {"idLog": 18, "time": "2026-01-21 14:05", "balanceChange": -12.30},
    {"idLog": 19, "time": "2026-01-21 19:50", "balanceChange": -85.00},
    {"idLog": 20, "time": "2026-01-22 08:00", "balanceChange": 3100.00},
    {"idLog": 21, "time": "2026-01-22 10:25", "balanceChange": -4.20},
    {"idLog": 22, "time": "2026-01-22 13:10", "balanceChange": -55.60},
    {"idLog": 23, "time": "2026-01-22 16:45", "balanceChange": 25.00},
    {"idLog": 24, "time": "2026-01-23 12:15", "balanceChange": -150.00},
    {"idLog": 25, "time": "2026-01-23 15:30", "balanceChange": -9.99}
    ])";

    userData = new userdata(this);
    ui->tableUserData->setModel(userData->getModel());
    userData->setUserData(testUserData);

    accountsData = new accountsdata(this);
    ui->tableAccountsData->setModel(accountsData->getModel());
    accountsData->setAccountsData(testAccountsData);

    cardData = new carddata(this);
    ui->tableCardData->setModel(cardData->getModel());
    cardData->setCardData(testCardData);

    logData = new logs(this);
    ui->tableLokit->setModel(logData->getModelAdmin());
    logData->setLog(testLogData);

    ui->tableUserData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(0, 60);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(1, 80);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(2, 80);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableUserData->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    ui->tableUserData->setColumnWidth(4, 60);

    //IdAccount only accept integers
    ui->lineTilitIdaccount->setValidator(new QIntValidator(0, 999999, this));
    //Balance and creditLimit only accept doubles of correct kind
    ui->lineTilitBalance->setValidator(new QDoubleValidator(0, 9999999999, 2, this));
    ui->lineTilitCreditlimit->setValidator(new QDoubleValidator(0, 9999999999, 2, this));
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableAccountsData->setColumnWidth(0, 80);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableAccountsData->setColumnWidth(1, 80);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableAccountsData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    ui->tableCardData->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(0, 80);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(2, 80);
    ui->tableCardData->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    ui->tableCardData->setColumnWidth(3, 80);

    ui->tableLokit->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    ui->tableLokit->setColumnWidth(0, 50);
    ui->tableLokit->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableLokit->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    connect(m_api, &ApiClient::userReceived, this, [this](QByteArray userInfo) {
        userData->setUserData(userInfo);
        userData->updateModel();
        //clears inputs
        ui->lineAsiakkaatID->clear();
        ui->lineAsiakkaatFname->clear();
        ui->lineAsiakkaatLname->clear();
        ui->lineAsiakkaatAddress->clear();
        ui->lineAsiakkaatRole->clear();
    });

    connect(m_api, &ApiClient::userCreated, this, [this](QString id) {
        //Just in case something goes wrong it doesnt crash with empty id
        if (!id.isEmpty()) {
            m_api->getUser(id);
        }
    });

    connect(m_api, &ApiClient::userUpdated, this, [this](QString id) {
        //Just in case something goes wrong it doesnt crash with empty id
        if (!id.isEmpty()) {
            m_api->getUser(id);
            //clears inputs
            ui->lineAsiakkaatID->clear();
            ui->lineAsiakkaatFname->clear();
            ui->lineAsiakkaatLname->clear();
            ui->lineAsiakkaatAddress->clear();
            ui->lineAsiakkaatRole->clear();
        }
    });

    connect(m_api, &ApiClient::userDeleted, this, [this]() {
        ui->lineAsiakkaatID->clear();
        //Clears the table after deleting
        userData->setUserData(QByteArray());
    });

    connect(m_api, &ApiClient::accountReceived, this, [this](QByteArray accountInfo) {
        accountsData->setAccountsData(accountInfo);
        accountsData->updateModel();
        //clears inputs
        ui->lineTilitIdaccount->clear();
        ui->lineTilitIduser->clear();
        ui->lineTilitBalance->clear();
        ui->lineTilitCreditlimit->clear();
    });

    connect(m_api, &ApiClient::accountCreated, this, [this](QByteArray accountInfo) {
        accountsData->setAccountsData(accountInfo);
        accountsData->updateModel();
        //clears inputs
        ui->lineTilitIdaccount->clear();
        ui->lineTilitIduser->clear();
        ui->lineTilitBalance->clear();
        ui->lineTilitCreditlimit->clear();
    });

    connect(m_api, &ApiClient::accountCreditUpdated, this, [this](int idAccount) {
        m_api->getAccount(idAccount);
        //clears inputs
        ui->lineTilitIdaccount->clear();
        ui->lineTilitIduser->clear();
        ui->lineTilitBalance->clear();
        ui->lineTilitCreditlimit->clear();
    });

    connect(m_api, &ApiClient::accountDeleted, this, [this] {
        //Clears the table after deleting
        accountsData->setAccountsData(QByteArray());
        //clears inputs
        ui->lineTilitIdaccount->clear();
        ui->lineTilitIduser->clear();
        ui->lineTilitBalance->clear();
        ui->lineTilitCreditlimit->clear();
    });
}

adminwindow::~adminwindow()
{
    delete ui;
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
}

void adminwindow::on_btnTilitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenTilit);
}

void adminwindow::on_btnKortitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenKortit);
}

void adminwindow::on_btnLokitLowBar_clicked()
{
    ui->stackedAdmin->setCurrentWidget(ui->screenLokit);
    ui->labelTiliID->setFocus();
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

        ui->lineAsiakkaatID->clear();
        ui->lineAsiakkaatFname->clear();
        ui->lineAsiakkaatLname->clear();
        ui->lineAsiakkaatAddress->clear();
        ui->lineAsiakkaatRole->clear();
    }
}


void adminwindow::on_btnLokitHae_clicked()
{

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

