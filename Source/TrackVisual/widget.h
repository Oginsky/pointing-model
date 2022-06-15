#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <Q3DScatter>

using namespace QtDataVisualization;

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget {
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void readTracks(QString filepath1, QString filepath2);

private slots:
    void on_input_btn_clicked();
    void show_track(QVector<QVector3D> our_track, QVector<QVector3D> target_track);

private:
    Ui::Widget *ui;
    QVector<QVector3D> our_track, target_track;

    Q3DScatter* graph;

    void readData(const QString& filepath, QVector<QVector3D>& track);
};
#endif // WIDGET_H
