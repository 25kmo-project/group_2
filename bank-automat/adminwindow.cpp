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
        ui->lineAsiakkaatID->clear();
        ui->lineAsiakkaatFname->clear();
        ui->lineAsiakkaatLname->clear();
        ui->lineAsiakkaatAddress->clear();
        ui->lineAsiakkaatRole->clear();
    });

    connect(m_api, &ApiClient::allUsersReceived, this, [this](QByteArray allUsersInfo) {
        userData->setUserData(allUsersInfo);
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

    connect(m_api, &ApiClient::allAccountsReceived, this, [this](QByteArray allAccountsInfo) {
        accountsData->setAccountsData(allAccountsInfo);
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

    connect(m_api, &ApiClient::allCardsReceived, this, [this](QByteArray allCards) {
        cardData->setCardData(allCards);
        ui->lineKortitIdcard->clear();
        ui->lineKortitLukossa->clear();
        ui->lineKortitPinYritykset->clear();
        ui->lineKortitIdUser->clear();
    });

    connect(m_api, &ApiClient::cardReceived, this, [this](QByteArray card) {
        cardData->setCardData(card);
        ui->lineKortitIdcard->clear();
        ui->lineKortitLukossa->clear();
        ui->lineKortitPinYritykset->clear();
        ui->lineKortitIdUser->clear();
        qDebug() << "Signal received";
    });

    connect(m_api, &ApiClient::adminLogsReceived, this, [this](QByteArray adminLogs) {
        logData->setLog(adminLogs);
        //clears input
        ui->lineLokitTiliID->clear();
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

        ui->lineAsiakkaatID->clear();
        ui->lineAsiakkaatFname->clear();
        ui->lineAsiakkaatLname->clear();
        ui->lineAsiakkaatAddress->clear();
        ui->lineAsiakkaatRole->clear();
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

}


void adminwindow::on_btnKorttiHaeKaikki_clicked()
{
    m_api->getAllCards();
}


void adminwindow::on_btnKorttiHaeKortti_clicked()
{
    QString idCard = ui->lineKortitIdcard->text().trimmed();
    if (!idCard.isEmpty()) {
        m_api->getCard(idCard);
    }
}

