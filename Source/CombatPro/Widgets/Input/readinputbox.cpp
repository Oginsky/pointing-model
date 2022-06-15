#include "readinputbox.h"

#include <QVector>
#include <QString>

#include "qfiledialog.h"

#include "Algo/combat.h"
#include "Algo/pnv.h"

#include "Tools/Reader.h"

ReadInputBox::ReadInputBox(PNVInfo* pnvInfo, QVector<LA>* our_trace, QVector<LA>* target_trace, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	this->pnvInfo = pnvInfo;
	this->our_trace = our_trace;
	this->target_trace = target_trace;
	
	unload_style = "QLabel {"
					"border-width: 1px;"
					"border-radius: 5px;"
					"border-color: rgb(255, 3, 62);"
					"border-style: solid;"
					"};";
					
	load_style = "QLabel {"
					"border-width: 1px;"
					"border-radius: 5px;"
					"border-color: rgb(6, 159, 90);"
					"border-style: solid;"
					"};";

	ui.read_pnv_btn->setEnabled(false);	// Не зная участков нельзя проинициализировать ПНВ, поэтому нужно сначала считать информацию об участках

	is_area_load = is_pnv_load = false;
	
}

ReadInputBox::~ReadInputBox()
{

}

bool ReadInputBox::isAreaLoad() { return is_area_load; }

bool ReadInputBox::isPnvLoad() { return is_pnv_load; }

bool ReadInputBox::isTraceLoad() { return (ui.our_trace_label->text() == "Загружено" 
											&& ui.target_trace_label->text() == "Загружено"); }

void ReadInputBox::getFilePaths(QString& _area_filepath, QString _pnv_filepath) {
    _area_filepath = this->area_filepath;
    _pnv_filepath = this->pnv_filepath;

}

void ReadInputBox::on_read_area_btn_clicked() {
	QString filename = QFileDialog::getOpenFileName(0, "Choose File: area", "", "");
	bool result = Reader::read_area(filename, *pnvInfo);

	
	if(result) {
        area_filepath = filename;
		ui.area_label->setText(QString::fromLocal8Bit("Загружено"));
		ui.area_label->setStyleSheet(load_style);
		ui.area_line->setText(filename);

		ui.read_pnv_btn->setEnabled(true);
		emit show_info_loaded();
	}
	else {
		ui.area_label->setText(QString::fromLocal8Bit("Не загружено"));
		ui.area_label->setStyleSheet(unload_style);

		ui.read_pnv_btn->setEnabled(false);
	}

	is_area_load = result;
	
}

void ReadInputBox::on_read_pnv_btn_clicked() {
	QString filename = QFileDialog::getOpenFileName(0, "Choose File: pnv", "", "");
	bool result = Reader::read_pnv(filename, *pnvInfo);

	if(result) {
        pnv_filepath = filename;
		ui.pnv_label->setText(QString::fromLocal8Bit("Загружено"));
		ui.pnv_label->setStyleSheet(load_style);
		ui.pnv_line->setText(filename);
	}
	else {
		ui.pnv_label->setText(QString::fromLocal8Bit("Не загружено"));
		ui.pnv_label->setStyleSheet(unload_style);
	}

	is_pnv_load = result;
}

void ReadInputBox::on_read_angp_btn_clicked() {
	QString filename = QFileDialog::getOpenFileName(0, "Choose File: our trace", "", "");
	bool result = Reader::read_angp(filename, *our_trace);

	if(result) {
		ui.our_trace_label->setText(QString::fromLocal8Bit("Загружено"));
		ui.our_trace_label->setStyleSheet(load_style);
		ui.our_trace_line->setText(filename);
	}
	else {
		ui.our_trace_label->setText(QString::fromLocal8Bit("Не загружено"));
		ui.our_trace_label->setStyleSheet(unload_style);

	}


	filename = QFileDialog::getOpenFileName(0, "Choose File: target trace", "", "");
	result = Reader::read_angp(filename, *target_trace);

	if(result) {
		ui.target_trace_label->setText(QString::fromLocal8Bit("Загружено"));
		ui.target_trace_label->setStyleSheet(load_style);
		ui.target_trace_line->setText(filename);
	}
	else {
		ui.target_trace_label->setText(QString::fromLocal8Bit("Не загружено"));
		ui.target_trace_label->setStyleSheet(unload_style);
	}


}
