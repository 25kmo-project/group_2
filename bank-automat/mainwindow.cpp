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
#include "ui_mainwindow.h"

#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include "ui_mainwindow.h"
#include <QLineEdit>

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
    api->setBaseUrl(QUrl("http://127.0.0.1:3000"));

    // Handle successful login
    connect(api, &ApiClient::loginSucceeded, this,
        [this](const LoginResultDto& loginResult)
        {
            // Hide any previous error message
            ui->errorLabel->setVisible(false);
            
            // Open the account selection window
            accountSelectWindow = new accountselect(loginResult, api, nullptr);
            accountSelectWindow->setAttribute(Qt::WA_DeleteOnClose);
            accountSelectWindow->showMaximized();
            
            // Close the login window
            this->close();
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
