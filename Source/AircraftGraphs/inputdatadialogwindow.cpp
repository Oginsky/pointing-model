#include "inputdatadialogwindow.h"
#include "ui_inputdatadialogwindow.h"

#include <qdebug.h>

#include <sstream>


InputDataDialogWindow::InputDataDialogWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputDataDialogWindow)
{
    ui->setupUi(this);
}

InputDataDialogWindow::~InputDataDialogWindow()
{
    delete ui;
}

void InputDataDialogWindow::setLists(QVector<MonitorParam>* airplane, QVector<MonitorParam>* target) {
    this->airplane = airplane;
    this->target = target;
}

void InputDataDialogWindow::on_buttonBox_accepted()
{
    QString data = ui->dataInput->toPlainText();
    this->successReading == readData(data);
}

bool InputDataDialogWindow::readData(QString data) {

    std::stringstream ss(data.toStdString());
    QVector<double> tmp1(7), tmp2(7);

    airplane->clear();
    target->clear();

    while(!ss.eof()) {
        int i = 0;
        while(i != 7 && ss >> tmp1[i++]);
        i = 1;
        tmp2[0] = tmp1[0];
        while(i != 7 && ss >> tmp2[i++]);

        airplane->push_back(MonitorParam(tmp1));
        target->push_back(MonitorParam(tmp2));
    }
    return true;
}
