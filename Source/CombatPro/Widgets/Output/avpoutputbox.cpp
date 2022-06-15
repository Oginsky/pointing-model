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

	areaTable->setItem(0, 0, new QTableWidgetItem(QString::fromLocal8Bit("Участок")));
	areaTable->setItem(0, 1, new QTableWidgetItem(QString::fromLocal8Bit("Время начала")));
	areaTable->setItem(0, 2, new QTableWidgetItem(QString::fromLocal8Bit("Начальная высота")));
	areaTable->setItem(0, 3, new QTableWidgetItem(QString::fromLocal8Bit("Начальная скорость")));

	areaTable->setStyleSheet("color: white; border-color: white;");
	//areaTable->resizeColumnsToContents();
    areaTable->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::Stretch);


	pnvNumberLabel = new QLabel(QString::fromLocal8Bit("Номер ПНВ: "));
    areasNumberLabel = new QLabel(QString::fromLocal8Bit("Участки: "));

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
    pnvNumberLabel->setText( QString::fromLocal8Bit("Номер ПНВ: ") + QString::number(pnvInfo.pnv_number) );

    QString areas;
    for(size_t i = 0; i < size_t(pnvInfo.pnvPath[number].size()); ++i) {
        if(pnvInfo.pnvPath[number][i] == nullptr) areas.push_back(QString::fromLocal8Bit("БУ, "));
        else areas.push_back(QString::number( pnvInfo.getAreaNumber(pnvInfo.pnvPath[number][i]) ) + ", ");
    }
    areas.resize(areas.size() - 2); // Чтобы убрать последнюю запятую с пробелом

    areasNumberLabel->setText(QString::fromLocal8Bit("Участки: ") + areas);

}
