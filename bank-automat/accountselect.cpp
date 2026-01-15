#include "accountselect.h"
#include "ui_accountselect.h"
#include <QPainter>
#include <QPixmap>


accountselect::accountselect(QString message, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::accountselect),
    cardnumber(message)
{
    ui->setupUi(this);
    ui->labelTest->setText(cardnumber);
}

accountselect::~accountselect()
{
    delete ui;
}

//valintaruudun tausta, sama kuin kirjautumissivulla
void accountselect::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    static const QPixmap selectPix(":/images/backgroundLogin.png");
    painter.drawPixmap(rect(), selectPix);
}
