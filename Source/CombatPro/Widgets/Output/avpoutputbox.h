#ifndef AVPOUTPUTBOX_H
#define AVPOUTPUTBOX_H

#include <QWidget>

class QTableWidget;
class QLabel;

struct PNVInfo;

class AvpOutputBox : public QWidget
{
	Q_OBJECT

public:
	explicit AvpOutputBox(QWidget *parent = nullptr);
	~AvpOutputBox();

	QTableWidget* getAreaTable();
    void updatePnvNumber(const PNVInfo& pnvInfo);

private:
	QTableWidget* areaTable;
	QLabel* pnvNumberLabel;
    QLabel* areasNumberLabel;
	
};

#endif // AVPOUTPUTBOX_H
