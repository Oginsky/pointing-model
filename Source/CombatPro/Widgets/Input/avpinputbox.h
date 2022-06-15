#ifndef AVPINPUTBOX_H
#define AVPINPUTBOX_H

#include <QWidget>
#include "ui_avpinputbox.h"

struct PNVInfo;

class AvpInputBox : public QWidget
{
	Q_OBJECT

public:
	AvpInputBox(QWidget *parent = 0);
	~AvpInputBox();

	void avpInput(PNVInfo& pnvInfo);
    void getProgramSpeed(double& vn1, double& vn2, double& vn3, double& vn4);
	
private:
	Ui::AvpInputBox ui;


signals:
	void start_avp();

private slots:
	void on_start_btn_clicked();
};

#endif // AVPINPUTBOX_H
