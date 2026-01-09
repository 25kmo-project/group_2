#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <QPainter>
#include <QPixmap>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isSplashScreen(true)
{
    ui->setupUi(this);
    setMainControlsVisible(false);
    
    //testaus voi poistaa myöhemmin
    const bool exists = QFile(":/images/background.png").exists();
    qDebug() << "Resource :/images/background.png exists?" << exists;
    
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
    //testaus voi poistaa myöhemmin
    qDebug() << "Siirtyminen pääruutuun";
    //näyttää login ruudun labelit ja napit
    setMainControlsVisible(true);
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
        QPixmap mainPix(":/images/backgroundLogin.png");
        painter.drawPixmap(rect(), mainPix);
    }
}

