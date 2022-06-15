#include "inputdatadialogwindow.h"
#include "ui_inputdatadialogwindow.h"

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


void InputDataDialogWindow::on_buttonBox_accepted()
{
    QString our_data = ui->ourDataInput->toPlainText();
    QString target_data = ui->targetDataInput->toPlainText();

    readData(our_data, target_data);
}

bool InputDataDialogWindow::readData(QString& our_data, QString& target_data) {

    std::stringstream ss(our_data.toStdString());
    QVector<QVector3D> our_track, target_track;

    while(!ss.eof()) {
        double x, y, z, temp;
        ss >> x >> y >> z >> temp;
        our_track.append(QVector3D(x, y, z));
    }

    ss.clear();
    ss.str(target_data.toStdString());
    while(!ss.eof()) {
        double x, y, z, temp;
        ss >> x >> y >> z >> temp;
        target_track.append(QVector3D(x, y, z));
    }

    emit send_track(our_track, target_track);

    return true;
}
