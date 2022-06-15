#include "graph.h"

IGraph::IGraph() {

}

IGraph::IGraph(QCustomPlot* graph) {
   this->graph = graph;
   this->graph->addGraph();
   this->graph->addGraph();

   QPen airplanePen, targetPen;
   airplanePen.setWidth(2);
   airplanePen.setColor(Qt::red);
   targetPen.setWidth(2);
   targetPen.setColor(Qt::blue);

   this->graph->graph(0)->setPen(airplanePen);
   this->graph->graph(1)->setPen(targetPen);

   graph->graph(0)->setScatterStyle(QCPScatterStyle::ScatterShape::ssDot);
   graph->graph(1)->setScatterStyle(QCPScatterStyle::ScatterShape::ssDot);
   graph->graph(0)->setLineStyle(QCPGraph::LineStyle::lsNone);
   graph->graph(1)->setLineStyle(QCPGraph::LineStyle::lsNone);


   airplanePoint = new QCPGraph(graph->xAxis, graph->yAxis);
   targetPoint = new QCPGraph(graph->xAxis, graph->yAxis);

   airplanePoint->setAdaptiveSampling(false);
   airplanePoint->setLineStyle(QCPGraph::lsNone);
   airplanePoint->setScatterStyle(QCPScatterStyle::ScatterShape::ssTriangle);
   airplanePoint->setPen(QPen(QBrush(Qt::blue), 2));
   targetPoint->setAdaptiveSampling(false);
   targetPoint->setLineStyle(QCPGraph::lsNone);
   targetPoint->setScatterStyle(QCPScatterStyle::ScatterShape::ssTriangle);
   targetPoint->setPen(QPen(QBrush(Qt::blue), 2));


//   QLinearGradient plotGradient;
//   plotGradient.setStart(0, 0);
//   plotGradient.setFinalStop(0, 350);
//   plotGradient.setColorAt(0, QColor(80, 80, 80));
//   plotGradient.setColorAt(1, QColor(50, 50, 50));
//   graph->setBackground(plotGradient);
//   QLinearGradient axisRectGradient;
//   axisRectGradient.setStart(0, 0);
//   axisRectGradient.setFinalStop(0, 350);
//   axisRectGradient.setColorAt(0, QColor(80, 80, 80));
//   axisRectGradient.setColorAt(1, QColor(30, 30, 30));
//   graph->axisRect()->setBackground(axisRectGradient);

//   graph->xAxis->setBasePen(QPen(Qt::white, 1));
//   graph->yAxis->setBasePen(QPen(Qt::white, 1));
//   graph->xAxis->setTickPen(QPen(Qt::white, 1));
//   graph->yAxis->setTickPen(QPen(Qt::white, 1));
//   graph->xAxis->setSubTickPen(QPen(Qt::white, 1));
//   graph->yAxis->setSubTickPen(QPen(Qt::white, 1));
//   graph->xAxis->setTickLabelColor(Qt::white);
//   graph->yAxis->setTickLabelColor(Qt::white);
//   graph->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//   graph->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
//   graph->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//   graph->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
//   graph->xAxis->grid()->setSubGridVisible(true);
//   graph->yAxis->grid()->setSubGridVisible(true);
//   graph->xAxis->grid()->setZeroLinePen(Qt::NoPen);
//   graph->yAxis->grid()->setZeroLinePen(Qt::NoPen);
//   graph->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
//   graph->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

//   graph->xAxis->setLabelColor(Qt::white);
//   graph->yAxis->setLabelColor(Qt::white);
}
IGraph::~IGraph() {

}

void IGraph::setGraphInfo(QString name, QString xAxisLabel, QString yAxisLabel) {
   this->name = name;
   this->xAxisName = xAxisLabel;
   this->yAxisName = yAxisLabel;

   this->graph->xAxis->setLabel(xAxisLabel);
   this->graph->yAxis->setLabel(yAxisName);
}
void IGraph::setGraphRange(double minx, double maxx, double miny, double maxy) {
    graph->xAxis->setRange(minx-minx/4, maxx+maxx/4);
    graph->yAxis->setRange(miny-miny/4, maxy+maxy/4);
}
void IGraph::setInteraction(QVector<QCP::Interaction>&& args) {
    for(auto& param: args)
        this->graph->setInteraction(param, true);

}
void IGraph::removePoints(double xPosAir, double yPosAir, double xPosTar, double yPosTar) {
    clearPoint();

    airplanePoint->addData(xPosAir, yPosAir);
    targetPoint->addData(xPosTar, yPosTar);

    graph->replot();
}
void IGraph::clearPoint() {
    airplanePoint->clearData();
    targetPoint->clearData();
}
