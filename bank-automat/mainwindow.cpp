#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QAction>
#include <QStyle>
#include <QLineEdit>


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

    QAction* toggleAction = ui->password->addAction(showIcon, QLineEdit::TrailingPosition);
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
    const auto controls = ui->centralwidget->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for (QWidget *w : controls) {
        w->setVisible(visible);
    }
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
    QString username = ui->user->text().trimmed();
    QString password = ui->password->text();

    // Testaa kovakoodatut tunnukset
    if (username == VALID_USERNAME && password == VALID_PASSWORD) {
        
        ui->welcomeLabel->setText("Kirjautuminen onnistui!");
        ui->welcomeLabel->setStyleSheet("QLabel { color: green; font-size: 16px; }");
        ui->welcomeLabel->setWordWrap(true); 
        ui->welcomeLabel->adjustSize();  
    } else {
        ui->welcomeLabel->setText("Virheellinen käyttäjätunnus tai salasana!");
        ui->welcomeLabel->setStyleSheet("QLabel { color: red; font-size: 16px; }");
        ui->welcomeLabel->setWordWrap(true);
        ui->welcomeLabel->adjustSize();  
        ui->password->clear();
        ui->password->setFocus();
    }
}
