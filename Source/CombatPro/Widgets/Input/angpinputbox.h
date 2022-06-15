#ifndef ANGPINPUTBOX_H
#define ANGPINPUTBOX_H

#include <QObject>
#include <QWidget>

#include "ui_angpinputbox.h"

enum MethodType { PEREHVAT, POGONYA, MANEVR, PEREH_MANEVR };

class AngpInputBox : public QWidget
{
	Q_OBJECT

public:
	AngpInputBox(QWidget *parent = 0);
	~AngpInputBox();

    void angpInput(double& dist, double& q, double& gm1, double& gm2, MethodType& method);
    bool is_optimal();

private:
	Ui::AngpInputBox ui;
	

signals:
    void aim(MethodType, double, double, double, double);

private slots: 
	void handleStart();
};

#endif // ANGPINPUTBOX_H
