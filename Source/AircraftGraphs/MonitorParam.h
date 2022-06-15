#ifndef MONITORPARAM_H
#define MONITORPARAM_H

#include <QVector>

struct MonitorParam {
    double t;
    double x, y, z;
    double v;
    double ksi, theta;

    MonitorParam() {}

    MonitorParam(const QVector<double>& param) {
        if(param.size() != 7) return;

        t = param[0];
        x =param[1];
        y = param[2];
        z = param[3];
        v = param[4];
        ksi = param[5];
        theta = param[6];
    }

};
#endif // MONITORPARAM_H
