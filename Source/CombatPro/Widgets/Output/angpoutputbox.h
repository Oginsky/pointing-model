#ifndef ANGPOUTPUTBOX_H
#define ANGPOUTPUTBOX_H

#include <QWidget>

namespace Ui {
class AngpOutputBox;
}

class AngpOutputBox : public QWidget
{
    Q_OBJECT

public:
    explicit AngpOutputBox(QWidget *parent = nullptr);
    ~AngpOutputBox();

    void updateInfo(double time, double dist, double kurs, double radius);
	void updateInfo(QString msg);

private:
    Ui::AngpOutputBox *ui;
};

#endif // ANGPOUTPUTBOX_H
