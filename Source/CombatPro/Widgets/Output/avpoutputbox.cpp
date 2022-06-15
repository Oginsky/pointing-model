#include "avpoutputbox.h"

#include <qtablewidget.h>
#include <qheaderview.h>
#include <QLabel>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <qstring.h>

#include "Algo/pnv.h"

AvpOutputBox::AvpOutputBox(QWidget *parent)
	: QWidget(parent)
{

	areaTable = new QTableWidget();

	areaTable->verticalHeader()->setVisible(false);
	areaTable->horizontalHeader()->setVisible(false);

	areaTable->setRowCount(1);
	areaTable->setColumnCount(4);

	areaTable->setItem(0, 0, new QTableWidgetItem(QString::fromLocal8Bit("�������")));
	areaTable->setItem(0, 1, new QTableWidgetItem(QString::fromLocal8Bit("����� ������")));
	areaTable->setItem(0, 2, new QTableWidgetItem(QString::fromLocal8Bit("��������� ������")));
	areaTable->setItem(0, 3, new QTableWidgetItem(QString::fromLocal8Bit("��������� ��������")));

	areaTable->setStyleSheet("color: white; border-color: white;");
	//areaTable->resizeColumnsToContents();
    areaTable->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::Stretch);


	pnvNumberLabel = new QLabel(QString::fromLocal8Bit("����� ���: "));
    areasNumberLabel = new QLabel(QString::fromLocal8Bit("�������: "));

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(pnvNumberLabel);
    vl->addWidget(areasNumberLabel);
	vl->addWidget(areaTable);

	setLayout(vl);
}

AvpOutputBox::~AvpOutputBox()
{

}

QTableWidget* AvpOutputBox::getAreaTable() {
	return areaTable;
}

void AvpOutputBox::updatePnvNumber(const PNVInfo& pnvInfo) {
    unsigned number = pnvInfo.pnv_number - 1;
    pnvNumberLabel->setText( QString::fromLocal8Bit("����� ���: ") + QString::number(pnvInfo.pnv_number) );

    QString areas;
    for(size_t i = 0; i < size_t(pnvInfo.pnvPath[number].size()); ++i) {
        if(pnvInfo.pnvPath[number][i] == nullptr) areas.push_back(QString::fromLocal8Bit("��, "));
        else areas.push_back(QString::number( pnvInfo.getAreaNumber(pnvInfo.pnvPath[number][i]) ) + ", ");
    }
    areas.resize(areas.size() - 2); // ����� ������ ��������� ������� � ��������

    areasNumberLabel->setText(QString::fromLocal8Bit("�������: ") + areas);

}
