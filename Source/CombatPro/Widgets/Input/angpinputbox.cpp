#include "angpinputbox.h"

#include "Algo/combat.h"

AngpInputBox::AngpInputBox(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	ui.manevr->setChecked(true);


	connect(ui.start, SIGNAL(clicked()), this, SLOT(handleStart()));
	
}

AngpInputBox::~AngpInputBox()
{

}

void AngpInputBox::handleStart() {

	double l = ui.distToTargetBox->value();
    double q = ui.q_zadBox->value()*d2r;
    double gm1 = ui.gamma1Box->value()*d2r;
    double gm2 = ui.gamma2Box->value()*d2r;

    MethodType method;
    if(ui.perehvat->isChecked()) method = PEREHVAT;
    else if(ui.pogonya->isChecked()) method = POGONYA;
    else if(ui.manevr->isChecked()) method = MANEVR;
    else method = PEREH_MANEVR;

    emit aim(method, l, q, gm1, gm2);

}

void AngpInputBox::angpInput(double& dist, double& q, double& gm1, double& gm2, MethodType& method) {
    dist = ui.distToTargetBox->value();
    q = ui.q_zadBox->value()*d2r;
    gm1 = ui.gamma1Box->value()*d2r;
    gm2 = ui.gamma2Box->value()*d2r;

    if(ui.perehvat->isChecked()) method = PEREHVAT;
    else if(ui.pogonya->isChecked()) method = POGONYA;
    else if(ui.manevr->isChecked()) method = MANEVR;
    else method = PEREH_MANEVR;
}

bool AngpInputBox::is_optimal() {
    return ui.optimal_box->isChecked();
}
