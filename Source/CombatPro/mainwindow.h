#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QMap>

#include "QCustomPlot/qcustomplot.h"

#include <vector>

#include "Algo\combat.h"
#include "Algo\manevr.h"
#include "Algo\pnv.h"
#include "Algo\pointingmodel.h"

#include "Widgets\Input\posinputbox.h"
#include "Widgets\Input\angpinputbox.h"
#include "Widgets\Input\avpinputbox.h"
#include "Widgets\Input\readinputbox.h"
#include "Widgets\Output\angpoutputbox.h"
#include "Widgets\Output\avpoutputbox.h"

#include <QTableWidget>
#include <QHeaderView>

using std::vector;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    /* �������� */
    PosInputBox* posBox;
    AngpInputBox* angpBox;
    AvpInputBox* avpBox;
    ReadInputBox* readBox;
    QWidget* aonBox;

    AngpOutputBox* angpOutputBox;
    AvpOutputBox* avpOutputBox;
    QTableWidget* areaOutputBox;

    /* ��� ���������� ����������� */
    QToolButton* previousClickInBtn, *previousClickOutBtn;
    QWidget* previousOpenInWidget, *previousOpenOutWidget;
    QMap<QToolButton*, QString> imageMap;


    // ��� ���������
    QTimer* timerAngp, *timerAvp;
    double currentTime, maxTime;
    size_t avp_cur_time, avp_max_time;
    QCPItemPixmap* icon;
    QCPItemText *speedText, *heightText;

    // ��� ����������� ������� ���������
    vector<TrackPoint> our_tp, target_tp;
    QCPCurve* our_line, *target_line;

    // ��� ����������� ������������� ��������� (�� ��������� ������)
    QVector<LA> our_trace, target_trace;
    QCPCurve* our_buildtrace_line, *target_buildtrace_line;

    // ��� �������� ������ ������
    PNVInfo pnvInfo;
    Model::ClimbProgramConsts climbProgramConsts;
    QVector<PNVPoint> pnv_trace;
    QCPCurve* pnv_line;

    // ��� ������� ������� ����������
    QSettings* settings;
    QProcess* trackVisualProcess, *aircraftGraphsProcess;
    Model::Track our_model_track, target_model_track;


    /* ������ */

    // ��������� ����������
    void configurateButtons();
    void configurateGraph();
    void configurateElements();
    void createAonBox();

    // ���������
    void setActive(QToolButton* btn, QString modifier = "");
    void setActiveBox(QWidget* box, QLayout* layout);
    void removePreviousWidget();
    void clearPlot();


signals:

private slots:

     /* ����� � �������� */
    void on_btn_in_click();
    void on_btn_out_click();
    void on_btn_apps_click();

    /* ������ ������� ��������� � �������������� ��������� */
    void start_angp(MethodType m, double l, double q, double gm1, double gm2);
    void slotTimerShowStep();	// ��� ��������� ��������� ����������

    /* ������ ��������� ������ ������ */
    void start_avp();
    void avp_timer_next_point();
    void show_area_info();

    /* ��� �������� ����������� */
    void copyOurInClipboard();
    void copyTargetInClipboard();
    void copyAircraftInClipboard();
    void buildInterceptTrajectory();

};
#endif // MAINWINDOW_H
