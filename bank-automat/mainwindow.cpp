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
        static const QPixmap mainPix(":/images/backgroundLogin.png");
        painter.drawPixmap(rect(), mainPix);

    }
}

