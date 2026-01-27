#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QAction>

#include "apiclient.h"
#include "accountselect.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
    protected:
    void paintEvent(QPaintEvent *event) override;
    
    private slots:
    void showMainScreen();
    void on_KirjauduButton_clicked();
    
    private:
    QAction* togglePasswordAction = nullptr;
    bool passwordVisible = false;

    Ui::MainWindow *ui;
    
    bool isSplashScreen;
    QTimer *splashTimer;
    
    ApiClient* api = nullptr;
    accountselect* accountSelectWindow = nullptr;
    
    void setMainControlsVisible(bool visible);
};

#endif
