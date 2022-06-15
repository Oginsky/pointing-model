#include "angpoutputbox.h"
#include "ui_angpoutputbox.h"

AngpOutputBox::AngpOutputBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AngpOutputBox)
{
    ui->setupUi(this);
}

AngpOutputBox::~AngpOutputBox()
{
    delete ui;
}

void AngpOutputBox::updateInfo(double time, double dist, double kurs, double radius) {
    ui->time_line->setText(QString::number(time));
    ui->dist_line->setText(QString::number(dist));
    ui->kurs_line->setText( (kurs != -1.0) ? QString::number(kurs) : "" );
    ui->radius_line->setText( (radius != -1.0) ? QString::number(radius) : "" );
}
void AngpOutputBox::updateInfo(QString msg) {
	 ui->time_line->setText(msg);
	 ui->dist_line->clear();
	 ui->kurs_line->clear();
	 ui->radius_line->clear();
}