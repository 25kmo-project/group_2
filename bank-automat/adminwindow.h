#ifndef ADMINWINDOW_H
#define ADMINWINDOW_H

#include <QWidget>

namespace Ui {
class adminwindow;
}

class adminwindow : public QWidget
{
    Q_OBJECT

public:
    explicit adminwindow(QString user, QWidget *parent = nullptr);
    ~adminwindow();

private:
    Ui::adminwindow *ui;
    QString user;

    class userdata *userData;
    class accountsdata *accountsData;

    QByteArray testUserData;
    QByteArray testAccountsData;

protected:
    void paintEvent(QPaintEvent *event) override;
private slots:
    void on_btnAsiakkaatLowBar_clicked();
    void on_btnTilitLowBar_clicked();
    void on_btnKortitLowBar_clicked();
};

#endif // ADMINWINDOW_H
