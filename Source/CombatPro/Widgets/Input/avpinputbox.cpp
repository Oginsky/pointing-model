#include "avpinputbox.h"

#include <QDoubleValidator>

#include "../../Algo/pnv.h"

AvpInputBox::AvpInputBox(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QDoubleValidator* validator = new QDoubleValidator(0, 1000, 0);
	ui.vn1_line->setValidator(validator);
	ui.vn2_line->setValidator(validator);
	ui.vn3_line->setValidator(validator);
	ui.vn4_line->setValidator(validator);

}

AvpInputBox::~AvpInputBox() {

}

void AvpInputBox::avpInput(PNVInfo& pnvInfo) {
	pnvInfo.VN1 = ui.vn1_line->text().toDouble();
	pnvInfo.VN2 = ui.vn2_line->text().toDouble();
	pnvInfo.VN3 = ui.vn3_line->text().toDouble();
	pnvInfo.VN4 = ui.vn4_line->text().toDouble();

	unsigned fly_mode_number = ui.fly_mode_comboBox->currentIndex();
	switch (fly_mode_number) {
		case 0: pnvInfo.fly_mode = FLY_MODE::CREISER; break;
		case 1: pnvInfo.fly_mode = FLY_MODE::COMBIN; break;
		case 2: pnvInfo.fly_mode = FLY_MODE::FORS; break;
	}
		
}
void AvpInputBox::getProgramSpeed(double& vn1, double& vn2, double& vn3, double& vn4) {
    vn1 = ui.vn1_line->text().toDouble();
    vn2 = ui.vn2_line->text().toDouble();
    vn3 = ui.vn3_line->text().toDouble();
    vn4 = ui.vn4_line->text().toDouble();
}


void AvpInputBox::on_start_btn_clicked() {
	emit start_avp();
}
