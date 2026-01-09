#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    //käytetään aloitusruudun näyttämiseen
    QTimer *splashTimer;
    bool isSplashScreen;
    //näyttää tai piilottaa pääruudun tekstit ja napit
    void setMainControlsVisible(bool visible);

private slots:
    void showMainScreen();

protected:
    void paintEvent(QPaintEvent *event) override;
};
#endif // MAINWINDOW_H
