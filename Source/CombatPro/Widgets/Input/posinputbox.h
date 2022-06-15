#ifndef POSINPUTBOX_H
#define POSINPUTBOX_H

#include <QObject>
#include <QWidget>

#include "ui_posinputbox.h"

#include "Algo/combat.h"

class PosInputBox : public QWidget
{
	Q_OBJECT

public:
	PosInputBox(QWidget *parent = 0);
	~PosInputBox();

	void inputs(LA& our, LA& target);


signals:

private slots:

private:
	Ui::PosInputBox ui;
};

#endif // POSINPUTBOX_H
