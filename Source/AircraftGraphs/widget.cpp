#include "widget.h"
#include "ui_widget.h"

#include <qdebug.h>

#include <stylesheets.h>

#include <cmath>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->PlotGraphsButton->setEnabled(false);
    //ui->playButton->setStyleSheet(StyleSheets::getPlayButtonStyleSheet());

    this->setWindowTitle("AircraftGraphs");

    horizontGraph = new IGraph(ui->graph1);
    verticalGraph = new IGraph(ui->graph3);
    climbGraph = new IGraph(ui->graph2);
    courseGraph = new IGraph(ui->graph4);
    horizontGraph->setGraphInfo("Horizont", "Z", "X");
    verticalGraph->setGraphInfo("Vertical", "X", "H");
    climbGraph->setGraphInfo("Набор Высоты", "TIME", "H");
    courseGraph->setGraphInfo("Курс", "TIME", "PSI");

    //ui->graph1->addGraph();
    //ui->graph1->addGraph();
    //ui->graph2->addGraph();
    //ui->graph2->addGraph();
    //ui->graph3->addGraph();
    //ui->graph3->addGraph();
    //ui->graph4->addGraph();
    //ui->graph4->addGraph();

    horizontGraph->setInteraction({QCP::iRangeDrag, QCP::iRangeZoom});
    verticalGraph->setInteraction({QCP::iRangeDrag, QCP::iRangeZoom});
    climbGraph->setInteraction({QCP::iRangeDrag, QCP::iRangeZoom});
    courseGraph->setInteraction({QCP::iRangeDrag, QCP::iRangeZoom});
   /* ui->graph1->setInteraction(QCP::iRangeDrag, true);
    ui->graph1->setInteraction(QCP::iRangeZoom, true);
    ui->graph2->setInteraction(QCP::iRangeDrag, true);
    ui->graph2->setInteraction(QCP::iRangeZoom, true);
    ui->graph3->setInteraction(QCP::iRangeDrag, true);
    ui->graph3->setInteraction(QCP::iRangeZoom, true);
    ui->graph4->setInteraction(QCP::iRangeDrag, true);
    ui->graph4->setInteraction(QCP::iRangeZoom, true);
*/

    ui->graph1Enable->setCheckState(Qt::CheckState::Checked);
    ui->graph2Enable->setCheckState(Qt::CheckState::Checked);
    ui->graph3Enable->setCheckState(Qt::CheckState::Checked);
    ui->graph4Enable->setCheckState(Qt::CheckState::Checked);

 /*  // currentAirplanePoint1 = new QCPGraph(ui->graph1->xAxis, ui->graph1->yAxis);
    currentAirplanePoint2 = new QCPGraph(ui->graph2->xAxis, ui->graph2->yAxis);
    currentAirplanePoint3 = new QCPGraph(ui->graph3->xAxis, ui->graph3->yAxis);
    currentAirplanePoint4 = new QCPGraph(ui->graph4->xAxis, ui->graph4->yAxis);

    //currentTargetPoint1 = new QCPGraph(ui->graph1->xAxis, ui->graph1->yAxis);
    currentTargetPoint2 = new QCPGraph(ui->graph2->xAxis, ui->graph2->yAxis);
    currentTargetPoint3 = new QCPGraph(ui->graph3->xAxis, ui->graph3->yAxis);
    currentTargetPoint4 = new QCPGraph(ui->graph4->xAxis, ui->graph4->yAxis);
*/
    ui->playStopGroup->setEnabled(false);

    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));

    createParametrsTable();
    //ui->tabWidget->setCurrentIndex(0);


}

void Widget::setPlotStyle() {

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_readFileButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(0, "Выберите данные ", "", "");
    if(filepath.isEmpty()) return;
    if(!readingFile(filepath))
        ui->statusReadingData->setText("Ошибка чтения данных");
        //qDebug() << "canno't read file";
    else {
       // qDebug() << "file was read";
        ui->statusReadingData->setText("Данные успешно загружены");
        ui->PlotGraphsButton->setEnabled(true);
        ui->playStopGroup->setEnabled(true);
    }


}

bool Widget::readingFile(QString filepath) {
    ifstream fin(filepath.toStdString());
    if(!fin.is_open()) {
        return false;
    }

    std::string line;
    QVector<double> tmp1(7), tmp2(7);
    std::stringstream ss;

    airplane.clear();
    target.clear();

    while(getline(fin, line)) {
       ss.str(line);

       int i = 0;
       while(i != 7 && fin >> tmp1[i++]);
       i = 1;
       tmp2[0] = tmp1[0];
       while(i != 7 && fin >> tmp2[i++]);

       airplane.push_back(MonitorParam(tmp1));
       target.push_back(MonitorParam(tmp2));

       ss.clear();
    }

    return true;
}

void Widget::on_setInputButton_clicked() {
    InputDataDialogWindow window;
    window.setLists(&this->airplane, &this->target);
    window.setModal(true);

        ui->statusReadingData->setText("Данные успешно загружены");
        ui->PlotGraphsButton->setEnabled(true);
        ui->playStopGroup->setEnabled(true);

    window.exec();

}


void Widget::on_PlotGraphsButton_clicked()
{
        auto aIt = airplane.begin();
        auto aEnd = airplane.end();
        auto tIt = target.begin();

        double maxX, maxY, maxZ, maxV, maxKsi, maxT;
        double minX, minY, minZ, minV, minKsi, minT;
        graphRange.x = graphRange.y = graphRange.z = graphRange.ksi = graphRange.v = -INFINITE;
        minRange.x = minRange.y = minRange.z = minRange.ksi = minRange.v = INFINITE;

        while(aIt != aEnd) {

            maxX = (aIt->x > tIt->x) ? aIt->x : tIt->x;
            minX = (aIt->x < tIt->x) ? aIt->x : tIt->x;
            graphRange.x = (graphRange.x > maxX) ? graphRange.x : maxX;
            minRange.x = (minRange.x < minX) ? minRange.x : minX;

            maxY = (aIt->y > tIt->y) ? aIt->y : tIt->y;
            minY = (aIt->y < tIt->y) ? aIt->y : tIt->y;
            graphRange.y = (graphRange.y > maxY) ? graphRange.y : maxY;
            minRange.y = (minRange.y < minY) ? minRange.y : minY;

            maxZ = (aIt->z > tIt->z) ? aIt->z : tIt->z;
            minZ = (aIt->z < tIt->z) ? aIt->z : tIt->z;
            graphRange.z = (graphRange.z > maxZ) ? graphRange.z : maxZ;
            minRange.z = (minRange.z < minZ) ? minRange.z : minZ;

            maxV = (aIt->v > tIt->v) ? aIt->v : tIt->v;
            minV = (aIt->v < tIt->v) ? aIt->v : tIt->v;
            graphRange.v = (graphRange.v > maxV) ? graphRange.v : maxV;
            minRange.v = (minRange.v < minV) ? minRange.v : minV;

            maxKsi = (aIt->ksi > tIt->ksi) ? aIt->ksi : tIt->ksi;
            minKsi = (aIt->ksi < tIt->ksi) ? aIt->ksi : tIt->ksi;
            graphRange.ksi = (graphRange.ksi > maxKsi) ? graphRange.ksi : maxKsi;
            minRange.ksi = (minRange.ksi < minKsi) ? minRange.ksi : minKsi;

            maxT = (aIt->t > tIt->t) ? aIt->t : tIt->t;
            minT = (aIt->t < tIt->t) ? aIt->t : tIt->t;
            graphRange.t = (graphRange.t > maxT) ? graphRange.t : maxT;
            minRange.t = (minRange.t < minT) ? minRange.t : minT;

            aIt++;
            tIt++;
        }

        plotGraphs();

}


void Widget::plotGraphs() {

    horizontGraph->clearPoint();
    verticalGraph->clearPoint();
    climbGraph->clearPoint();
    courseGraph->clearPoint();

    setGraphs(ui->graph1, Parametr::ZCOORD, Parametr::XCOORD);
    //setGraphInfo(ui->graph1, "Z", "X", graphRange.z, graphRange.x, minRange.z, minRange.x);
    horizontGraph->setGraphRange(minRange.z, graphRange.z, minRange.x, graphRange.x);
  //  ui->graph1->graph(0)->rescaleAxes(true);
    // ui->graph1->graph(1)->rescaleAxes(true);
    //ui->graph1->rescaleAxes(true);
    //setGraphsStyle(ui->graph1);r

    setGraphs(ui->graph2, Parametr::TIME, Parametr::YCOORD);
    climbGraph->setGraphRange(minRange.t, graphRange.t, minRange.y, graphRange.y);
    //setGraphInfo(ui->graph2, "T", "H", graphRange.t, graphRange.y, minRange.t, minRange.y);
    //setGraphsStyle(ui->graph2);

    setGraphs(ui->graph3, Parametr::XCOORD, Parametr::YCOORD);
    verticalGraph->setGraphRange(minRange.x, graphRange.x, minRange.y, graphRange.y);
    //setGraphInfo(ui->graph3, "X", "H", graphRange.x, graphRange.y, minRange.x, minRange.y);
    //setGraphsStyle(ui->graph3);

    courseGraph->setGraphRange(minRange.t, graphRange.t, minRange.ksi, graphRange.ksi);
    setGraphs(ui->graph4, Parametr::TIME, Parametr::KSI);
   // setGraphInfo(ui->graph4, "T", "KSI", graphRange.t, graphRange.ksi, minRange.t, minRange.ksi);
   // setGraphsStyle(ui->graph4);

    replots();

}
void Widget::replots() {
    ui->graph1->replot();
    ui->graph2->replot();
    ui->graph3->replot();
    ui->graph4->replot();
}

void Widget::setGraphs(QCustomPlot* graph, Parametr x, Parametr y) {
    graph->graph(0)->setData(Parameters::get(x, airplane), Parameters::get(y, airplane));
    graph->graph(1)->setData(Parameters::get(x, target), Parameters::get(y, target));
}

void Widget::setGraphInfo(QCustomPlot* graph, QString xAxisName, QString&& yAxisName, double rangeX, double rangeY, double mx, double my) {
    graph->xAxis->setLabel(xAxisName);
    graph->yAxis->setLabel(yAxisName);

    graph->xAxis->setRange(mx-mx/4, rangeX+ rangeX/4);
    graph->yAxis->setRange(my-my/4, rangeY+rangeY/4);
}
void Widget::setGraphsStyle(QCustomPlot* graph) {
    QPen airplanePen, targetPen;
    airplanePen.setWidth(2); targetPen.setWidth(2);
    airplanePen.setColor(Qt::green); targetPen.setColor(Qt::red);

    graph->graph(0)->setPen(airplanePen);
    graph->graph(1)->setPen(targetPen);

    graph->graph(0)->setScatterStyle(QCPScatterStyle::ScatterShape::ssDot);
    graph->graph(1)->setScatterStyle(QCPScatterStyle::ScatterShape::ssDot);
    graph->graph(0)->setLineStyle(QCPGraph::LineStyle::lsNone);
    graph->graph(1)->setLineStyle(QCPGraph::LineStyle::lsNone);
}

void Widget::on_rightStepButton_clicked()
{
    if(position >= airplane.size()-1) return;

    position++;

    //clearOldPoints();
    showPoints();
    //replots();

    showData();
    showParameters();
}

void Widget::on_leftStepButton_clicked()
{
    if(position <= 0) return;

    position--;

    //clearOldPoints();
    showPoints();
    //replots();

    showData();
    showParameters();

}
void Widget::showPoints() {
    horizontGraph->removePoints(airplane.at(position).z, airplane.at(position).x, target.at(position).z, target.at(position).x);
    verticalGraph->removePoints(airplane.at(position).x, airplane.at(position).y, target.at(position).x, target.at(position).y);
    climbGraph->removePoints(airplane.at(position).t, airplane.at(position).y, target.at(position).t, target.at(position).y);
    courseGraph->removePoints(airplane.at(position).t, airplane.at(position).ksi, target.at(position).t, target.at(position).ksi);

    /*
   // removePoint(currentAirplanePoint1, airplane.at(position).z, airplane.at(position).x);
    removePoint(currentAirplanePoint2, airplane.at(position).t, airplane.at(position).y);
    removePoint(currentAirplanePoint3, airplane.at(position).x, airplane.at(position).y);
    removePoint(currentAirplanePoint4, airplane.at(position).t, airplane.at(position).ksi);

   // removePoint(currentTargetPoint1, target.at(position).z, target.at(position).x);
    removePoint(currentTargetPoint2, target.at(position).t, target.at(position).y);
    removePoint(currentTargetPoint3, target.at(position).x, target.at(position).y);
    removePoint(currentTargetPoint4, target.at(position).t, target.at(position).ksi);
    */
}
void Widget::removePoint(QCPGraph* dwPoints, double xPosition, double yPosition) {
    dwPoints->setAdaptiveSampling(false);
    dwPoints->setLineStyle(QCPGraph::lsNone);
    dwPoints->setScatterStyle(QCPScatterStyle::ScatterShape::ssTriangle);
    dwPoints->setPen(QPen(QBrush(Qt::blue), 2));

    dwPoints->addData(xPosition, yPosition);
}

void Widget::clearOldPoints() {
   // currentAirplanePoint1->clearData();
    currentAirplanePoint2->clearData();
    currentAirplanePoint3->clearData();
    currentAirplanePoint4->clearData();

    //currentTargetPoint1->clearData();
    currentTargetPoint2->clearData();
    currentTargetPoint3->clearData();
    currentTargetPoint4->clearData();
}

void Widget::on_graph1Enable_stateChanged(int arg1)
{
    (arg1 == 0) ? ui->graph1->hide() : ui->graph1->show();
}

void Widget::on_graph3Enable_stateChanged(int arg1)
{
    (arg1 == 0) ? ui->graph3->hide() : ui->graph3->show();
}

void Widget::on_graph2Enable_stateChanged(int arg1)
{
    (arg1 == 0) ? ui->graph2->hide() : ui->graph2->show();
}

void Widget::on_graph4Enable_stateChanged(int arg1)
{
    (arg1 == 0) ? ui->graph4->hide() : ui->graph4->show();
}

void Widget::showData() {
    QString xStr = "X: " + QString::number(airplane.at(position).x) + "\t" + QString::number(target.at(position).x);
    QString yStr = "Y: " + QString::number(airplane.at(position).y) + "\t\t" + QString::number(target.at(position).y);
    QString zStr = "Z: " + QString::number(airplane.at(position).z) + "\t" + QString::number(target.at(position).z);
    QString tStr = "T: " + QString::number(airplane.at(position).t) + "\t\t" + QString::number(target.at(position).t);
    QString vStr = "V: " + QString::number(airplane.at(position).v) + "\t\t" + QString::number(target.at(position).v);
    QString ksiStr = "PSI: " + QString::number(airplane.at(position).ksi) + "\t" + QString::number(target.at(position).ksi);

    ui->xLabel->setText(xStr);
    ui->yLabel->setText(yStr);
    ui->zLabel->setText(zStr);
    ui->vLabel->setText(vStr);
    ui->ksiLabel->setText(ksiStr);
    ui->timeLabel->setText(tStr);

    ui->showTime->setText(QString::number(airplane.at(position).t));
}

void Widget::on_playButton_clicked()
{
    if(timer->isActive()) {
        ui->playButton->setStyleSheet(StyleSheets::getPlayButtonStyleSheet());
        timer->stop();
        return;
    }
    ui->playButton->setStyleSheet(StyleSheets::getStopButtonStyleSheets());
    timer->setInterval(10);
    timer->start();
}

void Widget::slotTimerAlarm() {

    if(position >= airplane.size()-1) {
        ui->playButton->setStyleSheet(StyleSheets::getPlayButtonStyleSheet());
        timer->stop();
        return;
    }
    position += log10(airplane.size());
    if(position >= airplane.size()) position = airplane.size()-1;

    //clearOldPoints();
    showPoints();
   // replots();
    showData();
    showParameters();


}

void Widget::on_showTime_editingFinished()
{

    int pos = ui->showTime->text().toInt();
    if(pos < 0 || pos >= airplane.size()) {
        ui->showTime->setText("#");
        return;
    }

    this->position = pos;
   // clearOldPoints();
    showPoints();
    showData();
    showParameters();
    //replots();
}
