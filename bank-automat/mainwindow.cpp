/* MainWindow, handles:
- displaying the splash screen at startup
- the login view (card number + PIN)
- the "show/hide" function for the PIN field
- logging in via the backend (ApiClient)
- moving to the account selection window after successful login
- drawing the background image itself*/

#include "mainwindow.h"
#include <QAction>
#include <QDebug>
#include <QFile>
#include <QLineEdit>
#include "adminwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>

#include <QPainter>
#include <QPixmap>
#include <QStyle>

// MainWindow is the first window shown when the application starts
// It displays a splash screen, then a login form and handles user authentication
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isSplashScreen(true) // Start in splash screen mode
{
    // Initialize UI components created by Qt Designer
    ui->setupUi(this);
    
    // Create an action for toggling password visibility
    togglePasswordAction = new QAction(this);
    togglePasswordAction->setIcon(QIcon(":/images/silmak.svg"));
    // Add the toggle icon to the password field (right side)
    ui->password->addAction(togglePasswordAction, QLineEdit::TrailingPosition);
    
    // Toggle password visibility when the icon is clicked
    connect(togglePasswordAction, &QAction::triggered, this, [this]() {
        passwordVisible = !passwordVisible;
        
        // Switch between hidden and visible password modes
        ui->password->setEchoMode(
            passwordVisible ? QLineEdit::Normal : QLineEdit::Password
        );
        
        // Update the icon based on visibility state
        togglePasswordAction->setIcon(
            QIcon(passwordVisible
                ? ":/images/silmaa.svg"
                : ":/images/silmak.svg")
            );
    });

    // Create the API client used for backend communication
    api = new ApiClient(this);

    // Handle successful login
    connect(api, &ApiClient::loginSucceeded, this,
        [this](const LoginResultDto& loginResult)
        {
            // Hide any previous error message
            ui->errorLabel->setVisible(false);
            
            if (loginResult.role == "admin") {
                // Open adminwindow
                adminWindow = new adminwindow(loginResult.idUser, api, nullptr);
                adminWindow->setAttribute(Qt::WA_DeleteOnClose);
                adminWindow->showMaximized();
            }
            else {
            // Open the account selection window
            accountSelectWindow = new accountselect(loginResult, api, inactivityTimer);
            accountSelectWindow->setAttribute(Qt::WA_DeleteOnClose);
            connect(accountSelectWindow, &QObject::destroyed, this, [this]() {
                accountSelectWindow = nullptr;
            });
            accountSelectWindow->showMaximized();
            }

            
            if (pinTimeoutTimer) pinTimeoutTimer->stop();
            // Close the login window
            this->hide();
        }
    );
    
    // Handle failed login or other API errors
    connect(api, &ApiClient::requestFailed, this,
        [this](const ApiError& error)
        {
            // Show backend-provided error message if available
            ui->errorLabel->setText(
                error.message.isEmpty()
                ? "Kirjautuminen epäonnistui"
                : error.message
            );
            ui->errorLabel->setVisible(true);
            
            // Clear PIN field and refocus it
            ui->password->clear();
            ui->password->setFocus();
        }
    );
    
    // Allow pressing Enter in either field to trigger login
    connect(ui->user, &QLineEdit::returnPressed,
        this, &MainWindow::on_KirjauduButton_clicked);

    connect(ui->password, &QLineEdit::returnPressed,
        this, &MainWindow::on_KirjauduButton_clicked);
            
    // Hide login controls while splash screen is shown
    setMainControlsVisible(false);
            
    // Setup splash screen timer (3 seconds)
    splashTimer = new QTimer(this);

    connect(splashTimer, &QTimer::timeout,
        this, &MainWindow::showMainScreen);
    splashTimer->start(3000);

    // PIN inactivity timer (10s), single-shot
    pinTimeoutTimer = new QTimer(this);
    pinTimeoutTimer->setSingleShot(true);
    pinTimeoutTimer->setInterval(10'000);

    connect(pinTimeoutTimer, &QTimer::timeout, this, &MainWindow::restartApplication);

    // Start timer only on first actual user edit in PIN field and reset on every subsequent edit
    connect(ui->password, &QLineEdit::textEdited, this, [this](const QString&) {
        armOrResetPinTimeout();
    });

    // Global inactivity timer (30s)
    inactivityTimer = new QTimer(this);
    inactivityTimer->setSingleShot(true);
    inactivityTimer->setInterval(30'000);

    connect(inactivityTimer, &QTimer::timeout,
        this, &MainWindow::returnToInitialState);

    // Start listening to all UI events
    qApp->installEventFilter(this);

    // Start timer immediately
    inactivityTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Called when the splash screen timeout expires
void MainWindow::showMainScreen()
{
    splashTimer->stop();
    isSplashScreen = false;
    // Show login UI
    setMainControlsVisible(true);
    // Trigger repaint to switch background image
    update();
}

// Show or hide main login controls
void MainWindow::setMainControlsVisible(bool visible)
{
    ui->loginCard->setVisible(visible);
    ui->errorLabel->setVisible(false);
    
    if (visible) {
        // Stop any previous PIN timeout; it must not run until first keypress
        if (pinTimeoutTimer) pinTimeoutTimer->stop();
        // Reset password state when login screen becomes visible
        passwordVisible = false;
        ui->password->clear();
        ui->password->setEchoMode(QLineEdit::Password);
        togglePasswordAction->setIcon(QIcon(":/images/silmak.svg"));
        
        // Focus the user/card input field
        ui->user->setFocus();
    }
}

// Slot connected to the login button
void MainWindow::on_KirjauduButton_clicked()
{
    ui->errorLabel->setVisible(false);
    
    // Read and trim user input
    const QString idCard = ui->user->text().trimmed();
    const QString pin    = ui->password->text().trimmed();
    
    // Send login request to backend
    api->login(idCard, pin);

}

// Custom paint event to draw the background image
void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    
    QPixmap bg;
    if (isSplashScreen) {
        // Background shown during splash screen
        bg.load(":/images/background.png");
    } else {
        // Background shown on login screen
        bg.load(":/images/backgroundLogin.png");
    }
    
    // Scale and draw background to fill the window
    painter.drawPixmap(rect(), bg);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type()) {
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::KeyPress:
        case QEvent::Wheel:
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
            resetInactivityTimer();
            break;
        default:
            break;
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::armOrResetPinTimeout()
{
    // Start on first keypress, reset on every keypress
    if (pinTimeoutTimer->isActive()) {
        pinTimeoutTimer->stop();
    }
    pinTimeoutTimer->start();
}

void MainWindow::restartApplication()
{
    // Best-effort: start new instance, then quit current
    const QString program = QCoreApplication::applicationFilePath();
    const QStringList args = QCoreApplication::arguments();

    const bool started = QProcess::startDetached(program, args);
    if (!started) {
        // Fallback: if restart fails, at least reset UI to start/login state
        ui->user->clear();
        ui->password->clear();
        ui->errorLabel->setVisible(false);
        ui->user->setFocus();
        return;
    }

    QCoreApplication::quit();
}

void MainWindow::resetInactivityTimer()
{
    if (!inactivityTimer) return;

    if (inactivityTimer->isActive())
        inactivityTimer->stop();

    inactivityTimer->start();
}

void MainWindow::returnToInitialState()
{
    // Ensure MainWindow is visible before closing other windows, otherwise the app may quit when the last visible window closes
    isSplashScreen = false;
    this->showMaximized();

    // Clear login
    setMainControlsVisible(true);
    ui->user->clear();
    ui->password->clear();
    ui->errorLabel->setVisible(false);
    ui->user->setFocus();

    // Now close all other top-level windows
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (w && w != this) w->close();
    }

    inactivityTimer->start();
}
