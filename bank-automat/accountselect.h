#ifndef ACCOUNTSELECT_H
#define ACCOUNTSELECT_H

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

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // ACCOUNTSELECT_H
