#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QList>

#include "MonitorParam.h"

enum Parametr {TIME, XCOORD, YCOORD, ZCOORD, V, KSI};

class Parameters
{
public:
    Parameters();


    static QVector<double> get(Parametr parametr, const QVector<MonitorParam>& ob);


};

#endif // PARAMETERS_H
