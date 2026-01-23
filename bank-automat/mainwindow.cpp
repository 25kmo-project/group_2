#include "mainwindow.h"
#include <QAction>
#include <QDebug>
#include <QFile>
#include <QLineEdit>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isSplashScreen(true)
{
    ui->setupUi(this);

    // lisää mahdollisuus painaa enteriä kirjautumiseen
    connect(ui->user, &QLineEdit::returnPressed, this, &MainWindow::on_KirjauduButton_clicked);
    connect(ui->password, &QLineEdit::returnPressed, this, &MainWindow::on_KirjauduButton_clicked);

    // Lisää salasanakentälle näytä/piilota nappi
    QIcon showIcon(":/images/silmaa.svg");
    QIcon hideIcon(":/images/silmak.svg");

    QAction *toggleAction = ui->password->addAction(showIcon, QLineEdit::TrailingPosition);
    toggleAction->setCheckable(true);

    connect(toggleAction, &QAction::toggled, [this, toggleAction, showIcon, hideIcon](bool checked) {
        if (checked) {
            ui->password->setEchoMode(QLineEdit::Normal);
            toggleAction->setIcon(hideIcon);
        } else {
            ui->password->setEchoMode(QLineEdit::Password);
            toggleAction->setIcon(showIcon);
        }
    });

    //piilota pääruudun tekstit ja napit aluksi
    setMainControlsVisible(false);

    // ajastin alku logolle
    splashTimer = new QTimer(this);
    connect(splashTimer, &QTimer::timeout, this, &MainWindow::showMainScreen);
    splashTimer->start(3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMainScreen()
{
    isSplashScreen = false;
    splashTimer->stop();
    //näyttää login ruudun labelit ja napit ja siirtää kursorin käyttäjä kenttään
    setMainControlsVisible(true);
    ui->user->setFocus();
    // Päivittää näkymän
    update();
}

void MainWindow::setMainControlsVisible(bool visible)
{
    ui->loginCard->setVisible(visible);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    if (isSplashScreen) {
        // Näytä aloitusruudun tausta
        static const QPixmap splashPix(":/images/background.png");
        painter.drawPixmap(rect(), splashPix);

    } else {
        // Näytä pääruudun tausta
        static const QPixmap mainPix(":/images/backgroundLogin.png");
        painter.drawPixmap(rect(), mainPix);
    }
}

void MainWindow::on_KirjauduButton_clicked()
{
    ui->errorLabel->setVisible(false); // piilota vanha virhe

    username = ui->user->text().trimmed();
    QString password = ui->password->text();

    // Testaa kovakoodatut tunnukset
    if (username == VALID_USERNAME && password == VALID_PASSWORD) {
        ui->errorLabel->setVisible(false);
        selectTimer = new QTimer(this);
        connect(selectTimer, &QTimer::timeout, this, &MainWindow::openSelectWindow);
        selectTimer->setSingleShot(true);
        selectTimer->start(1000);
    } else {
        ui->errorLabel->setText("Virheellinen käyttäjätunnus tai salasana");
        ui->errorLabel->setVisible(true);

        ui->password->clear();
        ui->password->setFocus();
    }
}

//tilinvalintaruudun avaaminen
void MainWindow::openSelectWindow()
{
    accountSelectWindow = new accountselect(username, nullptr);
    //nulpptr niin voidaan mainwindow sammuttaa ja accountselect säilyy
    accountSelectWindow->setAttribute(Qt::WA_DeleteOnClose);
    //Lisätään että muisti vapautetaan oikein suljettaessa, koska nullptr käytössä
    accountSelectWindow->showMaximized();
    this->close();      //suljetaan mainWindow
}
