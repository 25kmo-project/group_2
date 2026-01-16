#ifndef ACCOUNTSELECT_H
#define ACCOUNTSELECT_H

#include "account.h"
#include <QDialog>

namespace Ui {
class accountselect;
}

class accountselect : public QDialog
{
    Q_OBJECT

public:
    explicit accountselect(QString message, QWidget *parent = nullptr);
    ~accountselect();

private:
    Ui::accountselect *ui;
    QString cardnumber;
    QString cardtype;
    account *accountWindow;

private slots:
    void openAccountWindow();
    void on_btnSelectCredit_clicked();
    void on_btnSelectDebit_clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ACCOUNTSELECT_H
