#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>

#include "inputdatadialogwindow.h"

#include <algorithm>
#include <cmath>

using std::for_each;
using std::max;
using std::min;

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    graph = new Q3DScatter();
    QWidget* container = QWidget::createWindowContainer(graph);
    ui->graph_layout->addWidget(container);

    graph->activeTheme()->setType(Q3DTheme::ThemeDigia);
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFront);

    graph->setAxisX(new QValue3DAxis);
    graph->setAxisY(new QValue3DAxis);
    graph->setAxisZ(new QValue3DAxis);

    graph->axisX()->setRange(-850000, 850000);
    graph->axisY()->setRange(0, 250000);
    graph->axisZ()->setRange(-850000, 850000);

    QScatter3DSeries *series = new QScatter3DSeries;
    series->setItemLabelFormat(QStringLiteral("@xLabel, @yLabel, @zLabel"));
    series->setMesh(QAbstract3DSeries::Mesh::MeshPoint);
    series->setItemSize(0.05f);
    series->setBaseColor(QColor(Qt::red));
    graph->addSeries(series);

    QScatter3DSeries *series2 = new QScatter3DSeries;
    series2->setItemLabelFormat(QStringLiteral("@xLabel, @yLabel, @zLabel"));
    series2->setMesh(QAbstract3DSeries::Mesh::MeshPoint);
    series2->setItemSize(0.05f);
    series2->setBaseColor(QColor(Qt::GlobalColor::blue));
    graph->addSeries(series2);
}

void Widget::readTracks(QString filepath1, QString filepath2) {
    readData(filepath1, our_track);
    readData(filepath2, target_track);

    show_track(our_track, target_track);
}
void Widget::readData(const QString& filepath, QVector<QVector3D> &track) {
    QFile file(filepath);
    if(!file.open(QIODevice::ReadOnly)) return;

    QTextStream fin(&file);

    static auto values_to_numbers = [](const QStringList& values){
        double x, y, z;
        x = values[0].toDouble();
        y = values[1].toDouble();
        z = values[2].toDouble();

        return std::move(QVector3D(x, y, z));
    };


    while(!fin.atEnd()) {
        QStringList values = fin.readLine().split(" ");
        track.append(values_to_numbers(values));
    }

}

Widget::~Widget()
{
    delete ui;
}


void Widget::on_input_btn_clicked() {
    InputDataDialogWindow* window = new InputDataDialogWindow();
    window->setModal(true);
    window->show();
    connect(window, SIGNAL(send_track(QVector<QVector3D>, QVector<QVector3D>)), this, SLOT(show_track(QVector<QVector3D>, QVector<QVector3D>)));
}

void Widget::show_track(QVector<QVector3D> our_track, QVector<QVector3D> target_track) {
    // Add data from the QVector to datamodel
    QScatterDataArray *dataArray = new QScatterDataArray;
    dataArray->resize(our_track.count());
    QScatterDataItem *ptrToDataArray = &dataArray->first();
    for (int i = 0; i < our_track.count(); i++) {
        ptrToDataArray->setPosition(our_track.at(i));
        ptrToDataArray++;
    }

    graph->seriesList().at(0)->dataProxy()->resetArray(dataArray);

    QScatterDataArray* targetDataArray = new QScatterDataArray;
    targetDataArray->resize(target_track.count());
    QScatterDataItem* ptrToTargetDataArray = &targetDataArray->first();
    for (int i = 0; i < int(target_track.count()); i++) {
        ptrToTargetDataArray->setPosition(target_track.at(i));
        ptrToTargetDataArray++;
    }

    graph->seriesList().at(1)->dataProxy()->resetArray(targetDataArray);

    double MAX = std::numeric_limits<double>::max(),
           MIN = std::numeric_limits<double>::min();
    double maxX(MIN), maxY(MIN), maxZ(MIN);
    double minX(MAX), minY(MAX), minZ(MAX);

    for_each(our_track.begin(), our_track.end(), [&](QVector3D point) {
       maxX = max(maxX, (double)point.x());
       maxY = max(maxY, (double)point.y());
       maxZ = max(maxZ, (double)point.z());

       minX = min(minX, (double)point.x());
       minY = min(minY, (double)point.y());
       minZ = min(minZ, (double)point.z());

    });

    for_each(target_track.begin(), target_track.end(), [&](QVector3D point) {
       maxX = max(maxX, (double)point.x());
       maxY = max(maxY, (double)point.y());
       maxZ = max(maxZ, (double)point.z());

       minX = min(minX, (double)point.x());
       minY = min(minY, (double)point.y());
       minZ = min(minZ, (double)point.z());

    });

    minX = min(minX, minZ);
    maxX = max(maxX, maxZ);
    graph->axisX()->setRange(minX-10000, maxX+10000);
    graph->axisY()->setRange(minY, maxY+10000/*minX/2.0, maxX/2.0*/);
    graph->axisZ()->setRange(minX-10000, maxX+10000);


}
