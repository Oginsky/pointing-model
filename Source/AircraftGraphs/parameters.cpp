#include "parameters.h"

Parameters::Parameters()
{

}


QVector<double> Parameters::get(Parametr parametr, const QVector<MonitorParam>& ob) {
    QVector<MonitorParam>::ConstIterator it = ob.constBegin();
    QVector<MonitorParam>::ConstIterator end = ob.constEnd();
    QVector<double> prms(ob.size());
    int i = 0;

    while(it != end) {
        switch(parametr) {
            case Parametr::TIME:
                prms[i] = it->t;
                break;
            case Parametr::XCOORD:
                prms[i] = it->x;
                break;
            case Parametr::YCOORD:
                prms[i] = it->y;
               break;
           case Parametr::ZCOORD:
               prms[i] = it->z;
               break;
           case Parametr::KSI:
               prms[i] = it->ksi;
               break;
        }
        i++;

        it++;
    }

    return prms;

}
