#ifndef READINPUTBOX_H
#define READINPUTBOX_H

#include <QWidget>
#include "ui_readinputbox.h"

struct PNVInfo;
struct LA;

class ReadInputBox : public QWidget
{
	Q_OBJECT

public:
    ReadInputBox(PNVInfo* pnvInfo, QVector<LA>* our_trace, QVector<LA>* target_trace, QWidget *parent = 0);
	~ReadInputBox();
	
	bool isAreaLoad();
	bool isPnvLoad();
	bool isTraceLoad();

    void getFilePaths(QString& _area_filepath, QString _pnv_filepath);

private:
	Ui::ReadInputBox ui;
	bool is_area_load, is_pnv_load;
    QString area_filepath, pnv_filepath;
	
	PNVInfo* pnvInfo;
	QVector<LA>* our_trace, *target_trace;
	
	QString unload_style, load_style;


signals:
	void show_info_loaded();

private slots:
	void on_read_area_btn_clicked();
	void on_read_pnv_btn_clicked();
	void on_read_angp_btn_clicked();
};

#endif // READINPUTBOX_H
