#ifndef ACCOUNTSELECT_H
#define ACCOUNTSELECT_H

#include <QDialog>
#include <QMap>

#include "apiclient.h"

namespace Ui {
    class accountselect;
}

class accountselect : public QDialog
{
    Q_OBJECT
    
    public:
    explicit accountselect(
        const LoginResultDto& loginResult,
        ApiClient* api,
        QWidget *parent = nullptr
    );
    ~accountselect();
    
    protected:
    void paintEvent(QPaintEvent *event) override;
    
    private slots:
    void on_btnSelectDebit_clicked();
    void on_btnSelectCredit_clicked();
    
    private:
    Ui::accountselect *ui;
    
    ApiClient* m_api;
    LoginResultDto m_login;
    
    QString selectedAccountType;
    QMap<QString, int> accountIdByType;
    
    void openAccountWindow();
};

#endif
