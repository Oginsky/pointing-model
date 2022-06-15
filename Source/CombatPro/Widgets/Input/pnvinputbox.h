#ifndef PNVINPUTBOX_H
#define PNVINPUTBOX_H

#include <QWidget>
#include "ui_pnvinputbox.h"

class PnvInputBox : public QWidget
{
	Q_OBJECT

public:
	PnvInputBox(QWidget *parent = 0);
	~PnvInputBox();

private:
	Ui::PnvInputBox ui;
};

#endif // PNVINPUTBOX_H
