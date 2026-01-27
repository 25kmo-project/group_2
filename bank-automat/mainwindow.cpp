#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPixmap>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
, isSplashScreen(true)
{
    ui->setupUi(this);
    
    togglePasswordAction = new QAction(this);
    togglePasswordAction->setIcon(QIcon(":/images/silmak.svg"));
    ui->password->addAction(togglePasswordAction, QLineEdit::TrailingPosition);
    
    connect(togglePasswordAction, &QAction::triggered, this, [this]() {
        passwordVisible = !passwordVisible;
        
        ui->password->setEchoMode(
            passwordVisible ? QLineEdit::Normal : QLineEdit::Password
        );
        
        togglePasswordAction->setIcon(
            QIcon(passwordVisible
                ? ":/images/silmaa.svg"
                : ":/images/silmak.svg")
            );
    });

    api = new ApiClient(this);
    api->setBaseUrl(QUrl("http://127.0.0.1:3000"));

    connect(api, &ApiClient::loginSucceeded, this,
        [this](const LoginResultDto& loginResult)
        {
            ui->errorLabel->setVisible(false);
            
            accountSelectWindow = new accountselect(loginResult, api, nullptr);
            accountSelectWindow->setAttribute(Qt::WA_DeleteOnClose);
            accountSelectWindow->showMaximized();
            
            this->close();
        }
    );
    
    connect(api, &ApiClient::requestFailed, this,
        [this](const ApiError& error)
        {
            ui->errorLabel->setText(
                error.message.isEmpty()
                ? "Kirjautuminen epäonnistui"
                : error.message
            );
            ui->errorLabel->setVisible(true);
            
            ui->password->clear();
            ui->password->setFocus();
        }
    );
    
    connect(ui->user, &QLineEdit::returnPressed,
        this, &MainWindow::on_KirjauduButton_clicked);
        connect(ui->password, &QLineEdit::returnPressed,
            this, &MainWindow::on_KirjauduButton_clicked);
            
            setMainControlsVisible(false);
            
            splashTimer = new QTimer(this);
            connect(splashTimer, &QTimer::timeout,
                this, &MainWindow::showMainScreen);
                splashTimer->start(3000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showMainScreen()
{
    splashTimer->stop();
    isSplashScreen = false;
    setMainControlsVisible(true);
    update();
}

void MainWindow::setMainControlsVisible(bool visible)
{
    ui->loginCard->setVisible(visible);
    ui->errorLabel->setVisible(false);
    
    if (visible) {
        passwordVisible = false;
        ui->password->clear();
        ui->password->setEchoMode(QLineEdit::Password);
        togglePasswordAction->setIcon(QIcon(":/images/silmak.svg"));
        
        ui->user->setFocus();
    }
}

void MainWindow::on_KirjauduButton_clicked()
{
    ui->errorLabel->setVisible(false);
    
    const QString idCard = ui->user->text().trimmed();
    const QString pin    = ui->password->text().trimmed();
    
    api->login(idCard, pin);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    
    QPixmap bg;
    if (isSplashScreen) {
        bg.load(":/images/background.png");
    } else {
        bg.load(":/images/backgroundLogin.png");
    }
    
    painter.drawPixmap(rect(), bg);
}
