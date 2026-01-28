#include "adminwindow.h"
#include "accountsdata.h"
#include "ui_adminwindow.h"
#include "userdata.h"
#include <qpainter.h>

adminwindow::adminwindow(QString user, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::adminwindow),
    user(user)
{
    ui->setupUi(this);
    ui->labelAdminUser->setText("Adminkäyttäjä: "+user);
    ui->stackedAdmin->setCurrentWidget(ui->screenAsiakkaat);

    //testidataa käyttäjätaulua varten
    testUserData = R"([
    {"iduser": "1", "fname": "Aku", "lname": "Ankka", "streetaddress": "Paratiisitie 13"},
    {"iduser": "2", "fname": "Roope", "lname": "Ankka", "streetaddress": "Rahasäiliö 1"},
    {"iduser": "3", "fname": "Mikki", "lname": "Hiiri", "streetaddress": "Jokikatu 43"},
    {"iduser": "4", "fname": "Hessu", "lname": "Hopo", "streetaddress": "Koivukatu 7"}
    ])";

    testAccountsData = R"([
    {"idaccount": 1, "iduser": "1", "balance": 500, "credilimit": 0},
    {"idaccount": 2, "iduser": "2", "balance": 1000000, "creditlimit": 0},
    {"idaccount": 3, "iduser": "2", "balance": 10000, "creditlimit": 50000},
    {"idaccount": 4, "iduser": "3", "balance": 25000, "creditlimit": 0},
    {"idaccount": 5, "iduser": "3", "balance": 250, "creditlimit": 1500},
    {"idaccount": 6, "iduser": "4", "balance": 1000, "creditlimit": 0}
    ])";

    userData = new userdata(this);
    ui->tableUserData->setModel(userData->getModel());
    userData->setUserData(testUserData);

    accountsData = new accountsdata(this);
    ui->tableAccountsData->setModel(accountsData->getModel());
    accountsData->setAccountsData(testAccountsData);
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

