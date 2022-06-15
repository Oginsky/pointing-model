#ifndef INPUTDATADIALOGWINDOW_H
#define INPUTDATADIALOGWINDOW_H

#include <QDialog>

#include "MonitorParam.h"

namespace Ui {
class InputDataDialogWindow;
}

class InputDataDialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InputDataDialogWindow(QWidget *parent = nullptr);
    ~InputDataDialogWindow();

    void setLists(QVector<MonitorParam>* airplane, QVector<MonitorParam>* target);
    bool successReading = false;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputDataDialogWindow *ui;

    QVector<MonitorParam>* airplane;
    QVector<MonitorParam>* target;

    bool readData(QString data);
};

#endif // INPUTDATADIALOGWINDOW_H
