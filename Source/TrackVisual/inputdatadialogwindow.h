#ifndef INPUTDATADIALOGWINDOW_H
#define INPUTDATADIALOGWINDOW_H

#include <QDialog>
#include <QVector>
#include <QVector3D>

#include "struct.h"


namespace Ui {
class InputDataDialogWindow;
}

class InputDataDialogWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InputDataDialogWindow(QWidget *parent = nullptr);
    ~InputDataDialogWindow();


signals:
    void send_track(QVector<QVector3D>, QVector<QVector3D>);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputDataDialogWindow* ui;


    bool readData(QString& our_data, QString& target_data);
};

#endif // INPUTDATADIALOGWINDOW_H
