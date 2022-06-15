#include "widget.h"
#include "ui_widget.h"

#include <vector>
#include "matfunction.h"

using std::vector;

void Widget::createParametrsTable() {
    model = new QStandardItemModel();
    ui->parametersTable->setModel(model);
    ui->parametersTable->verticalHeader()->setVisible(false);
    model->setHorizontalHeaderItem(0, new QStandardItem("Характеристики положения"));
    model->setHorizontalHeaderItem(1, new QStandardItem("ЛА1"));
    model->setHorizontalHeaderItem(2, new QStandardItem("ЛА2"));

    model->setRowCount(26);

    QString titles[] = {"Дальность ракета-цель",
                        "Верт. угл визирования", "Гориз. угл визирования",
                        "Производ. дальн. ракеат-цель",
                        "Верт. угл линии визир.", "Гориз. угл линии визир.",
                        "Гориз. ракурс цели",
                        "Рад. сост-ия скорости цели",
                        "Производная x (1)", "Производная y (1)", "Производная z (1)",
                        "Производная x (2)", "Производная y (2)", "Производная z (2)",
                        "Признак полусферы",
                        "Проекция вектора скорости\nна оси антенной с.к. x(1)",
                        "...y (1)", "... z (1)",
                        "... x (2)", "... y (2)", "... z (3)",
                        "Угл. скорость линии визирования"};

    for(int i = 0; i < 23; i++)
        model->setItem(i, 0, new QStandardItem(titles[i]));

    ui->parametersTable->setColumnWidth(0, 200);
}

void Widget::showParameters() {
    double d21;
    double ev, eg;
    double dt21;
    double fv, fg;
    double qr2;
    double v2r;
    double xt1, yt1, zt1;
    double xt2, yt2, zt2;
    int ipps;
    double vx1l, vy1l, vz1l;
    double vx2l, vy2l, vz2l;
    double wlb;

    double ippsD;
    double* ippsPtr = &ippsD;

    double x1(airplane[position].x), y1(airplane[position].y), z1(airplane[position].z);
    double x2(target[position].x), y2(target[position].y), z2(target[position].z);
    double psi1(airplane[position].ksi), psi2(target[position].ksi);
    double v1(airplane[position].v), v2(target[position].v);
    double tttv1(airplane[position].theta), tttv2(target[position].theta);

    QVector<double*> ptr{&d21, &ev, &eg, &dt21, &fv, &fg, &qr2, &v2r, &xt1, &yt1, &zt1, &xt2, &yt2, &zt2,
                ippsPtr, &vx1l, &vy1l, &vz1l, &vx2l, &vy2l, &vz2l, &wlb};

    par(x1,y1,z1,x2,y2,z2,psi1,psi2,v1,v2,tttv1,tttv2,
        d21,
        ev,eg,
        dt21,
        fv,fg,
        qr2,
        v2r,
        xt1,yt1,zt1,xt2,yt2,zt2,
        ipps,
        vx1l, vy1l, vz1l, vx2l, vy2l, vz2l, wlb);

    ippsD = ipps;

    for(int i = 0; i < ptr.size(); ++i)
        model->setItem(i, 1, new QStandardItem(QString::number(*ptr[i])));

   /* model->setItem(0,1, new QStandardItem(QString::number(d21)));
    model->setItem(1,1, new QStandardItem(QString::number(ev)));
    model->setItem(2,1, new QStandardItem(QString::number(eg)));
    model->setItem(3,1, new QStandardItem(QString::number(dt21)));
    model->setItem(4,1, new QStandardItem(QString::number(fv)));
    model->setItem(5,1, new QStandardItem(QString::number(fg)));
    model->setItem(6,1, new QStandardItem(QString::number(qr2)));
    model->setItem(7,1, new QStandardItem(QString::number(v2r)));
    model->setItem(8,1, new QStandardItem(QString::number(xt1)));
    model->setItem(9,1, new QStandardItem(QString::number(yt1)));
    model->setItem(10,1, new QStandardItem(QString::number(zt1)));
    model->setItem(11,1, new QStandardItem(QString::number(xt2)));
    model->setItem(12,1, new QStandardItem(QString::number(yt2)));
    model->setItem(13,1, new QStandardItem(QString::number(zt2)));
    model->setItem(14,1, new QStandardItem(QString::number(ipps)));
    model->setItem(15,1, new QStandardItem(QString::number(vx1l)));
    model->setItem(16,1, new QStandardItem(QString::number(vy1l)));
    model->setItem(17,1, new QStandardItem(QString::number(vz1l)));
    model->setItem(18,1, new QStandardItem(QString::number(vx2l)));
    model->setItem(19,1, new QStandardItem(QString::number(vy2l)));
    model->setItem(20,1, new QStandardItem(QString::number(vz2l)));
    model->setItem(21,1, new QStandardItem(QString::number(wlb)));
*/

    par(x2,y2,z2,x1,y1,z1,psi2,psi1,v2,v1,tttv2,tttv1,
        d21,
        ev,eg,
        dt21,
        fv,fg,
        qr2,
        v2r,
        xt1,yt1,zt1,xt2,yt2,zt2,
        ipps,
        vx1l, vy1l, vz1l, vx2l, vy2l, vz2l, wlb);

    ippsD = ipps;

    for(int i = 0; i < ptr.size(); ++i)
        model->setItem(i, 2, new QStandardItem(QString::number(*ptr[i])));

/*    model->setItem(0,2, new QStandardItem(QString::number(d21)));
    model->setItem(1,2, new QStandardItem(QString::number(ev)));
    model->setItem(2,2, new QStandardItem(QString::number(eg)));
    model->setItem(3,2, new QStandardItem(QString::number(dt21)));
    model->setItem(4,2, new QStandardItem(QString::number(fv)));
    model->setItem(5,2, new QStandardItem(QString::number(fg)));
    model->setItem(6,2, new QStandardItem(QString::number(qr2)));
    model->setItem(7,2, new QStandardItem(QString::number(v2r)));
    model->setItem(8,2, new QStandardItem(QString::number(xt1)));
    model->setItem(9,2, new QStandardItem(QString::number(yt1)));
    model->setItem(10,2, new QStandardItem(QString::number(zt1)));
    model->setItem(11,2, new QStandardItem(QString::number(xt2)));
    model->setItem(12,2, new QStandardItem(QString::number(yt2)));
    model->setItem(13,2, new QStandardItem(QString::number(zt2)));
    model->setItem(14,2, new QStandardItem(QString::number(ipps)));
    model->setItem(15,2, new QStandardItem(QString::number(vx1l)));
    model->setItem(16,2, new QStandardItem(QString::number(vy1l)));
    model->setItem(17,2, new QStandardItem(QString::number(vz1l)));
    model->setItem(18,2, new QStandardItem(QString::number(vx2l)));
    model->setItem(19,2, new QStandardItem(QString::number(vy2l)));
    model->setItem(20,2, new QStandardItem(QString::number(vz2l)));
    model->setItem(21,2, new QStandardItem(QString::number(wlb)));
*/
}

