#include "posinputbox.h"

PosInputBox::PosInputBox(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

PosInputBox::~PosInputBox()
{

}

void PosInputBox::inputs(LA& our, LA& target) {
	our.x = ui.XApBox->value();
	our.y = ui.HeightApBox->value();
	our.z = ui.ZApBox->value();

	//double kurs = ui.KursApBox->value();
	//kurs = (kurs > 270 && kurs < 360) ? (kurs - 360)*d2r : kurs*d2r; 
	//our.psi = kurs*d2r;

	our.psi = ui.KursApBox->value() * d2r;
	our.theta = ui.AngApBox->value() * d2r;
	our.v = ui.SpeedAprBox->value();

	target.x = ui.XTarBox->value();
	target.y = ui.HeightTarBox->value();
	target.z = ui.ZTarBox->value();
	target.psi = ui.KursTarBox->value() * d2r;
	target.theta = ui.AngTarBox->value() * d2r;
	target.v = ui.SpeedTarBox->value();
}
