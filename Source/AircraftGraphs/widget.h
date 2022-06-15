#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <QString>
#include <QVector>
#include <QList>

#include <QFileDialog>
#include <fstream>
#include <sstream>

#include <QTimer>

#include "MonitorParam.h"
#include "parameters.h"
#include "qcustomplot.h"
#include "inputdatadialogwindow.h"
#include "graph.h"

using std::ifstream;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void slotTimerAlarm();

    void on_readFileButton_clicked();
    void on_setInputButton_clicked();

    void on_PlotGraphsButton_clicked();

    void on_graph1Enable_stateChanged(int arg1);
    void on_graph3Enable_stateChanged(int arg1);
    void on_graph2Enable_stateChanged(int arg1);
    void on_graph4Enable_stateChanged(int arg1);

    void on_playButton_clicked();
    void on_rightStepButton_clicked();
    void on_leftStepButton_clicked();

    void on_showTime_editingFinished();

private:
    Ui::Widget *ui;

    void setPlotStyle();

    bool readingFile(QString filepath);
    void plotGraphs();
    void replots();
    void removePoint(QCPGraph* dwPoints, double xPosition, double yPosition);
    void clearOldPoints();
    void showPoints();
    void setGraphs(QCustomPlot* graph, Parametr x, Parametr y);
    void setGraphsStyle(QCustomPlot* graph);
    void setGraphInfo(QCustomPlot* graph, QString xAxisName, QString&& yAxisName, double rangeX, double rangeY, double mx, double my);

    void showData();
    void createParametrsTable();
    void showParameters();


    QTimer* timer;

    QVector<MonitorParam> airplane;
    QVector<MonitorParam> target;
    MonitorParam graphRange;
    MonitorParam minRange;

    IGraph* horizontGraph;
    IGraph* verticalGraph;
    IGraph* climbGraph;
    IGraph* courseGraph;

    QCPGraph* currentAirplanePoint1;
    QCPGraph* currentAirplanePoint2;
    QCPGraph* currentAirplanePoint3;
    QCPGraph* currentAirplanePoint4;
    QCPGraph* currentTargetPoint1;
    QCPGraph* currentTargetPoint2;
    QCPGraph* currentTargetPoint3;
    QCPGraph* currentTargetPoint4;

    QStandardItemModel* model;
    int position = -1;

};
#endif // WIDGET_H



