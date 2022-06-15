#ifndef IGraph_H
#define IGraph_H

#include "qcustomplot.h"

class IGraph {
  private:
    IGraph();

    QCustomPlot* graph;
    QString name;
    QString xAxisName, yAxisName;

    QCPGraph* airplanePoint;
    QCPGraph* targetPoint;


    public:
           IGraph(QCustomPlot* graph);
           ~IGraph();

        void setGraphInfo(QString name, QString xAxisLabel, QString yAxisLabel);
        void setGraphRange(double minx, double maxx, double miny, double maxy);
        void setInteraction(QVector<QCP::Interaction>&& args);
        void removePoints(double xPosAir, double yPosAir, double xPosTar, double yPosTar);
        void clearPoint();
};



#endif // IGraph_H
