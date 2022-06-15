#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QHeaderView>

#include <algorithm>

#include "Tools/Reader.h"


using std::for_each;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainToolBar->setVisible(false);
    ui->statusBar->setVisible(false);

    settings = new QSettings("config.ini", QSettings::Format::IniFormat);

    // Инициализация элементов
    posBox = new PosInputBox();
    angpBox = new AngpInputBox();
    avpBox = new AvpInputBox();
    readBox = new ReadInputBox(&pnvInfo, &our_trace, &target_trace);

    angpOutputBox = new AngpOutputBox();
    avpOutputBox = new AvpOutputBox();
    areaOutputBox = new QTableWidget();

    timerAngp = new QTimer();
    timerAvp = new QTimer();

    trackVisualProcess = new QProcess(this);
    aircraftGraphsProcess = new QProcess(this);

    // Установка первоначальных форм
    ui->pos_layout->addWidget(posBox);

    previousClickInBtn = ui->angp_in_btn;
    previousOpenInWidget = angpBox;
    ui->data_in_layout->addWidget(angpBox);

    previousClickOutBtn = ui->avp_out_btn;
    previousOpenOutWidget = avpOutputBox;
    ui->data_out_layout->addWidget(previousOpenOutWidget);

    // Установка связей в приложении
    configurateButtons();
    configurateGraph();
    configurateElements();
    createAonBox();

    connect(angpBox, SIGNAL(aim(MethodType,double,double,double,double)), this, SLOT(start_angp(MethodType, double, double, double, double)));
    connect(avpBox, SIGNAL(start_avp()), this, SLOT(start_avp()));
    connect(readBox, SIGNAL(show_info_loaded()), this, SLOT(show_area_info()));
    connect(timerAngp, SIGNAL(timeout()), this, SLOT(slotTimerShowStep()));
    connect(timerAvp, SIGNAL(timeout()), this, SLOT(avp_timer_next_point()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


/* Настройка приложений */
void MainWindow::configurateButtons() {
    connect(ui->angp_in_btn, SIGNAL(clicked()), this, SLOT(on_btn_in_click()));
    connect(ui->avp_in_btn, SIGNAL(clicked()), this, SLOT(on_btn_in_click()));
    connect(ui->read_in_btn, SIGNAL(clicked()), this, SLOT(on_btn_in_click()));
    connect(ui->aon_in_btn, SIGNAL(clicked()), this, SLOT(on_btn_in_click()));

    connect(ui->angp_out_btn, SIGNAL(clicked()), this, SLOT(on_btn_out_click()));
    connect(ui->avp_out_btn, SIGNAL(clicked()), this, SLOT(on_btn_out_click()));
    connect(ui->area_out_btn, SIGNAL(clicked()), this, SLOT(on_btn_out_click()));

    // Дополнителая информация для обновления иконок
    imageMap.insert(ui->angp_in_btn, "angp-in");
    imageMap.insert(ui->avp_in_btn, "avp-in");
    imageMap.insert(ui->read_in_btn, "read-in");
    imageMap.insert(ui->aon_in_btn, "aon-in");

    imageMap.insert(ui->angp_out_btn, "angp-out");
    imageMap.insert(ui->avp_out_btn, "avp-out");
    imageMap.insert(ui->area_out_btn, "area-out");
}
void MainWindow::configurateGraph() {
    ui->angpPlot->setInteraction(QCP::Interaction::iRangeDrag);
    ui->angpPlot->setInteraction(QCP::Interaction::iRangeZoom);

    ui->avpPlot->setInteraction(QCP::Interaction::iRangeDrag);
    ui->avpPlot->setInteraction(QCP::Interaction::iRangeZoom);

    our_line = new QCPCurve(ui->angpPlot->xAxis, ui->angpPlot->yAxis);
    target_line = new QCPCurve(ui->angpPlot->xAxis, ui->angpPlot->yAxis);
    our_line->setPen(QPen(Qt::red, 3));
    target_line->setPen(QPen(Qt::blue, 3));

    our_buildtrace_line = new QCPCurve(ui->angpPlot->xAxis, ui->angpPlot->yAxis), target_buildtrace_line = new QCPCurve(ui->angpPlot->xAxis, ui->angpPlot->yAxis);
    our_buildtrace_line->setPen(QPen(QColor(255, 5, 51)));

    pnv_line = new QCPCurve(ui->avpPlot->xAxis, ui->avpPlot->yAxis);
    pnv_line->setPen(QPen(QColor(255, 5, 51)));

    ui->avpPlot->setVisible(false);


    // Установка стиля плота
/*	ui->angpPlot->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->angpPlot->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->angpPlot->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->angpPlot->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->angpPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->angpPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->angpPlot->xAxis->setTickLabelColor(Qt::white);
    ui->angpPlot->yAxis->setTickLabelColor(Qt::white);

    ui->angpPlot->setBackground(QBrush(QColor(54, 57, 63)));
*/

    // add label for phase tracer:
    speedText = new QCPItemText(ui->avpPlot);
    speedText->position->setType(QCPItemPosition::ptPlotCoords);
    speedText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    speedText->setTextAlignment(Qt::AlignLeft);
    speedText->setFont(QFont(font().family(), 9));
    speedText->setPadding(QMargins(8, 0, 0, 8));

    heightText = new QCPItemText(ui->avpPlot);
    heightText->position->setType(QCPItemPosition::ptPlotCoords);
    heightText->setPositionAlignment(Qt::AlignRight|Qt::AlignTop);
    heightText->setTextAlignment(Qt::AlignLeft);
    heightText->setFont(QFont(font().family(), 9));
    heightText->setPadding(QMargins(8, 0, 0, 16));

    // prepare x axis with country labels:
    QVector<double> ticks;
    QVector<QString> labels;
    for(unsigned height = 0; height <= 20000; height += 1000) ticks << height;
    for(unsigned i = 0; i < unsigned(ticks.size()); ++i) labels << QString::number(ticks[i]/1000) + QString::fromLocal8Bit(" КМ");

    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTicks(ticks, labels);
    ui->avpPlot->yAxis->setTicker(textTicker);
}
void MainWindow::configurateElements() {
    /* Для вывода информации о участках */
    areaOutputBox->verticalHeader()->setVisible(false);
    areaOutputBox->horizontalHeader()->setVisible(false);

    areaOutputBox->setRowCount(1);
    areaOutputBox->setColumnCount(7);

    areaOutputBox->setItem(0, 0, new QTableWidgetItem(QString::fromLocal8Bit("Участок")));
    areaOutputBox->setItem(0, 1, new QTableWidgetItem(QString::fromLocal8Bit("Начальная высота")));
    areaOutputBox->setItem(0, 2, new QTableWidgetItem(QString::fromLocal8Bit("Конечная высота")));
    areaOutputBox->setItem(0, 3, new QTableWidgetItem(QString::fromLocal8Bit("Начальная скорость")));
    areaOutputBox->setItem(0, 4, new QTableWidgetItem(QString::fromLocal8Bit("Конечная скорость")));
    areaOutputBox->setItem(0, 5, new QTableWidgetItem(QString::fromLocal8Bit("Ускорение")));
    areaOutputBox->setItem(0, 6, new QTableWidgetItem(QString::fromLocal8Bit("Вертикальная составляющая")));

    areaOutputBox->setStyleSheet("color: white; border-color: white;");
    areaOutputBox->horizontalHeader()->resizeSections(QHeaderView::ResizeMode::Stretch);

}

QString getQss(QString&& image_name) {
    QString icon = "image: url(:/image/Resources/image/";
    return std::move(QString("QToolButton { "
                         + icon + image_name + ".png);\n"
                         "icon-size: 24px;"
                         "border: none;"
                         "}"
                          "QToolButton::hover {"
                          + icon + image_name + "-press.png);\n"
                          "icon-size: 24px;"
                          "border: none;"
                          "}"
                         )
                     );
}

void MainWindow::createAonBox() {
    /* Создаем ui в коде */
    aonBox = new QWidget();

    QToolButton* copyOurBtn = new QToolButton(this);
    QToolButton* copyTargetBtn = new QToolButton(this);
    QToolButton* copyAircraftBtn = new QToolButton(this);
    QToolButton* trackVizualBtn = new QToolButton(this);
    trackVizualBtn->setObjectName("TrackVisualBtn");
    QToolButton* aircraftGraphsBtn = new QToolButton(this);
    aircraftGraphsBtn->setObjectName("AircraftGraphsBtn");

    copyOurBtn->setText("");
    copyTargetBtn->setText("");
    trackVizualBtn->setText("");
    aircraftGraphsBtn->setText("");

    copyOurBtn->setStyleSheet(getQss("copy"));
    copyTargetBtn->setStyleSheet(copyOurBtn->styleSheet());
    copyAircraftBtn->setStyleSheet(copyOurBtn->styleSheet());
    trackVizualBtn->setStyleSheet(getQss("trackvisual"));
    aircraftGraphsBtn->setStyleSheet(getQss("aircraft"));

    copyOurBtn->setToolTip(QString::fromLocal8Bit("Траектория перехватчика"));
    copyTargetBtn->setToolTip(QString::fromLocal8Bit("Траектория цели"));
    copyAircraftBtn->setToolTip(QString::fromLocal8Bit("Данные для Aircraft"));
    trackVizualBtn->setToolTip(QString::fromLocal8Bit("Открыть TrackVisual: Просмотр траектории в пространстве"));
    aircraftGraphsBtn->setToolTip(QString::fromLocal8Bit("Открыть AircraftGraphs: Просмотр характеристик наведения"));

    QPushButton* startBtn = new QPushButton(QString::fromLocal8Bit(("Построить траекторию")), this);
    startBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QCheckBox* auto_start_track_visual = new QCheckBox(QString::fromLocal8Bit("Запустить отображение траектории по окончанию расчетов"), this);
    auto_start_track_visual->setObjectName("AutoStartVisual");
    auto_start_track_visual->setChecked(true);
    auto_start_track_visual->setStyleSheet(std::move("color: white;"));

    connect( copyOurBtn, SIGNAL(clicked()), this, SLOT(copyOurInClipboard()) );
    connect( copyTargetBtn, SIGNAL(clicked()), this, SLOT(copyTargetInClipboard()) );
    connect( copyAircraftBtn, SIGNAL(clicked()), this, SLOT(copyAircraftInClipboard()) );
    connect( startBtn, SIGNAL(clicked()), this, SLOT(buildInterceptTrajectory()) );
    connect( trackVizualBtn, SIGNAL(clicked()), this, SLOT(on_btn_apps_click()));
    connect( aircraftGraphsBtn, SIGNAL(clicked()), this, SLOT(on_btn_apps_click()));

    QHBoxLayout* buttons_layout = new QHBoxLayout();
    QHBoxLayout* buttons_apps_layout = new QHBoxLayout();
    QVBoxLayout* content_layout = new QVBoxLayout();

    buttons_layout->setAlignment(Qt::AlignLeft);
    buttons_apps_layout->setAlignment(Qt::AlignLeft);
    content_layout->setContentsMargins(1, 1, 1, 0);

    buttons_layout->addWidget(copyOurBtn);
    buttons_layout->addWidget(copyTargetBtn);
    buttons_layout->addWidget(copyAircraftBtn);
    buttons_apps_layout->addWidget(trackVizualBtn);
    buttons_apps_layout->addWidget(aircraftGraphsBtn);

    content_layout->addWidget(startBtn);
    content_layout->addWidget(auto_start_track_visual);
    content_layout->addLayout(buttons_layout);
    content_layout->addLayout(buttons_apps_layout);

    aonBox->setLayout(content_layout);
}


/* Управление вкладками */
void MainWindow::on_btn_in_click() {
    QToolButton* sender = qobject_cast<QToolButton*>(QObject::sender());
    if(sender == previousClickInBtn) return;

    // Убираем предыдущую открытую вкладку
    previousOpenInWidget = ui->data_in_layout->itemAt(0)->widget();
    delete ui->data_in_layout->takeAt(0);
    ui->data_in_layout->removeWidget(previousOpenInWidget);
    previousOpenInWidget->setVisible(false);

    // Меняем дизайн предыдущей нажатой кнопки
    setActive(previousClickInBtn);

    //using namespace std::placeholders;  // for _1, _2, _3...
    //static auto setActiveInWidget = std::bind(&MainWindow::setActiveBox, _1, ui->data_in_layout);

    if(sender == ui->angp_in_btn) {
        setActiveBox(angpBox, ui->data_in_layout);
        // Переключаем плоты
        ui->angpPlot->setVisible(true);
        ui->avpPlot->setVisible(false);
    }
    else if(sender == ui->avp_in_btn) {
        setActiveBox(avpBox, ui->data_in_layout);
        // Переключаем плоты
        ui->angpPlot->setVisible(false);
        ui->avpPlot->setVisible(true);
    }
    else if(sender == ui->read_in_btn) setActiveBox(readBox, ui->data_in_layout);
    else if(sender == ui->aon_in_btn) setActiveBox(aonBox, ui->data_in_layout);

    setActive(sender, "active");
    previousClickInBtn = sender;

}
void MainWindow::on_btn_out_click() {
    QToolButton* sender = qobject_cast<QToolButton*>(QObject::sender());
    setActive(sender, "active");				// Чтобы снимать "notice" даже если нажали на ту же самую
    if(sender == previousClickOutBtn) return;

    // Убираем предыдущую открытую вкладку
    previousOpenOutWidget = ui->data_out_layout->itemAt(0)->widget();
    delete ui->data_out_layout->takeAt(0);
    ui->data_out_layout->removeWidget(previousOpenInWidget);
    previousOpenOutWidget->setVisible(false);

    // Меняем дизайн предыдущей нажатой кнопки
    setActive(previousClickOutBtn);

    if(sender == ui->angp_out_btn) setActiveBox(angpOutputBox, ui->data_out_layout);
    else if(sender == ui->avp_out_btn) setActiveBox(avpOutputBox, ui->data_out_layout);
    else if(sender == ui->area_out_btn) setActiveBox(areaOutputBox, ui->data_out_layout);

    setActive(sender, "active");
    previousClickOutBtn = sender;
}
void MainWindow::on_btn_apps_click() {
    QToolButton* sender = qobject_cast<QToolButton*>(QObject::sender());
    if(our_model_track.empty() || target_model_track.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("Модель не была запущена"),
                             QString::fromLocal8Bit("Чтобы продолжить нажмите \"Рассчитать\""));
        return;
    }

    if(sender->objectName() == "TrackVisualBtn") {
        if(trackVisualProcess->state() == QProcess::ProcessState::Running) {
            trackVisualProcess->close();
            delete trackVisualProcess;
        }

        settings->beginGroup("Paths");
        const QString our_track_path = settings->value("our_track").toString(),
                      target_track_path = settings->value("target_track").toString();
        Reader::write_track(our_track_path, our_model_track);
        Reader::write_track(target_track_path, target_model_track);
        settings->endGroup();

        settings->beginGroup("Apps");
        qDebug() << settings->value("TrackVisual").toString() << endl
                 << QDir::currentPath() + "\\our.txt" << endl
                 << QDir::currentPath() + "\\target.txt" << endl;

        trackVisualProcess = new QProcess(this);
        trackVisualProcess->setProgram(settings->value("TrackVisual").toString());
        trackVisualProcess->setArguments(QStringList()
                                         << our_track_path
                                         << target_track_path
                                         );
        settings->endGroup();

        trackVisualProcess->start();
    }
    else if(sender->objectName() == "AircraftGraphsBtn") {
        if(aircraftGraphsProcess->state() == QProcess::ProcessState::Running) {
            aircraftGraphsProcess->close();
            delete aircraftGraphsProcess;
        }

        settings->beginGroup("Apps");
        aircraftGraphsProcess = new QProcess(this);
        aircraftGraphsProcess->setProgram(settings->value("AircraftGraphs").toString());
        settings->endGroup();

        aircraftGraphsProcess->start();
    }

}


/* Управление интерфейсом */
void MainWindow::setActive(QToolButton* btn, QString modifier) {
    if(!btn) return;
    QString icon = "image: url(:/image/Resources/image/" + imageMap.value(btn)
                    + (modifier.isEmpty() ? "" : "-"+modifier) + ".png);";
    btn->setStyleSheet("QToolButton { "
                       + icon + "\n"
                       "icon-size: 24px;"
                       "border: none;"
                       "}");
}
void MainWindow::setActiveBox(QWidget* box, QLayout* layout) {
    layout->addWidget(box);
    box->setVisible(true);
}
void MainWindow::removePreviousWidget() {
    // Убираем предыдущую открытую вкладку
    previousOpenInWidget = ui->data_in_layout->itemAt(0)->widget();
    delete ui->data_in_layout->takeAt(0);
    ui->data_in_layout->removeWidget(previousOpenInWidget);
    previousOpenInWidget->setVisible(false);
}
void MainWindow::clearPlot() {
    our_buildtrace_line->data()->clear();
    target_buildtrace_line->data()->clear();
    our_line->data()->clear();
    target_line->data()->clear();
}


/* Запуск методов наведения в горизонтальной плоскости */
void MainWindow::start_angp(MethodType m, double l, double q, double gm1, double gm2) {
    if(timerAngp->isActive()) {
        timerAngp->stop();
        return;
    }

    LA our, target;
    posBox->inputs(our, target);

    LA a1, a2;
    posBox->inputs(a1, a2);
    Model::motionParamLA our2(a1.x, a1.y, a1.z, a1.v, a1.psi, a1.theta, 0.0);
    Model::motionParamLA target2(a2.x, a2.y, a2.z, a2.v, a2.psi, a2.theta, 0.0);

    // Очищаем предыдущий результат
    our_tp.clear(), target_tp.clear();
    our_line->data()->clear(), target_line->data()->clear();
    currentTime = 0;

    // Расчет по выбранному методу наведения
    double time = 0, dist = -1;
    double kurs = -1, radius = -1;
    bool possible = true;
    switch (m) {
        case PEREHVAT: {
            InterceptionInfo info = interception_dl(our, target, l);
            buildInterceptionTrack(our, info, our_tp);
            buildTargetTrack(target, info.getTime(), target_tp);
            time = info.getTime();
            break;
        }
        case POGONYA: {
            PursuitInfo pinfo = pursuit_dl(our, target, l);
            time = pinfo.time;
            buildPogonyaTrack(our, target, time, our_tp);
            buildTargetTrack(target, time, target_tp);
            break;
        }
        case MANEVR: {
            TwoManeuverInfo m2info = (angpBox->is_optimal()) ? optimal_manevr(our, target, gm1, gm2, q, l) : twoManeuver2(our, target, gm1, gm2, q, l);
            time = m2info.time; dist = m2info.lenght;
            radius = m2info.minfo1.getRadius();

            possible = m2info.possible;
            if(time == 0) return;
            buildTwoManeuvertrack(our, target, m2info, our_tp);
            buildTargetTrack(target, time, target_tp);
            break;
        }
        case PEREH_MANEVR: {
            ManeuverInfo minfo;
            InterceptionInfo iinfo;
            interception_maneuver(our, target, l, gm1, minfo, iinfo);
            kurs = iinfo.getCource();
            time = minfo.getNeedTime() + iinfo.getTime();

            buildManevrTrack2(our, minfo, our_tp);
            LA our2 = our;
            our.x = our_tp.back().x;
            our.y = our_tp.back().y;
            our.z = our_tp.back().z;
            buildInterceptionTrack(our, iinfo, our_tp, iinfo.getTime());
            buildTargetTrack(target, time, target_tp);

            break;
         }
    }

     // Выводим результаты расчетовы и сигнал о них
    if(!possible) {
        setActive(ui->angp_out_btn, "notice");
        angpOutputBox->updateInfo(QString::fromLocal8Bit("Нет решения"));
        return;
    }
    dist = target.v * time;
    angpOutputBox->updateInfo(time, dist, kurs*r2d, radius);
    setActive(ui->angp_out_btn, "notice");

    // Запуск отрисовки
    maxTime = time-1;
    timerAngp->setInterval(5);
    timerAngp->start();

}
void MainWindow::slotTimerShowStep() {
    if(currentTime > maxTime) {
        timerAngp->stop();
        return;
    }

    int index = std::floor(currentTime);
    if(index >= our_tp.size() || index >= target_tp.size()) {
         timerAngp->stop();
        return;
    }
    if(index < our_tp.size()) our_line->addData(index, our_tp[index].z, our_tp[index].x);
    if(index < target_tp.size()) target_line->addData(currentTime, target_tp[index].z, target_tp[index].x);

    //our_line->rescaleAxes(true);
    //target_line->rescaleAxes(true);
    double range = std::max(std::max(std::abs(our_tp[index].z),  std::abs(target_tp[index].z)), std::max(std::abs(our_tp[index].x),  std::abs(target_tp[index].x)));
    ui->angpPlot->xAxis->setRange(-range, range);
    ui->angpPlot->yAxis->setRange(-range, range);


    ui->angpPlot->replot();
    currentTime++;
}


/* Запуск программы набора высоты */
void MainWindow::start_avp() {
    // Проверка что ПНВ и Участки загружены, если нет переводит на окно загрузки
    if(!readBox->isAreaLoad() || !readBox->isPnvLoad()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("Данные не загружены"), QString::fromLocal8Bit("Загрузите Участки и ПНВ"));
        removePreviousWidget();
        setActiveBox(readBox, ui->data_in_layout);
        setActive(previousClickInBtn);
        setActive(ui->read_in_btn, "active");
        previousClickInBtn = ui->read_in_btn;
        return;
    }


    // Получаем входные данные
    avpBox->avpInput(pnvInfo);
    LA our, target;
    posBox->inputs(our, target);
    pnvInfo.kurs = our.psi; pnvInfo.theta = our.theta;

    // Определяем и строим ПНВ
    buildPnvTrack(our, target, pnvInfo, pnv_trace, pnvInfo.fly_mode);

    // Очищаем предыдущие результаты
    avp_cur_time = 0;
    avp_max_time = pnv_trace.size();
    pnv_line->data()->clear();
    avpOutputBox->getAreaTable()->setRowCount(1);

    ui->avpPlot->xAxis->setRange(0.0, pnv_trace.back().lenght);
    ui->avpPlot->yAxis->setRange(0.0, pnvInfo.maxH);

    avpOutputBox->updatePnvNumber(pnvInfo);
    setActive(ui->avp_out_btn, "notice");

    // Запуск отрисовки
    timerAvp->setInterval(15);
    timerAvp->start();

}
void MainWindow::avp_timer_next_point() {
    if(avp_cur_time >= avp_max_time) {
        timerAvp->stop();
        return;
    }

    if( pnv_trace[avp_cur_time].new_area) {
        QTableWidget* passAreaTable = avpOutputBox->getAreaTable();
        int rows = passAreaTable->rowCount();
        passAreaTable->setRowCount(rows + 1);
        passAreaTable->setItem(rows, 0, new QTableWidgetItem(QString::number(pnv_trace[avp_cur_time].area) ) );
        passAreaTable->setItem(rows, 1, new QTableWidgetItem(QString::number(pnv_trace[avp_cur_time].time) ) );
        passAreaTable->setItem(rows, 2, new QTableWidgetItem(QString::number(pnv_trace[avp_cur_time].height) ) );
        passAreaTable->setItem(rows, 3, new QTableWidgetItem(QString::number(pnv_trace[avp_cur_time].v) ) );
    }

    pnv_line->addData(avp_cur_time, pnv_trace[avp_cur_time].lenght, pnv_trace[avp_cur_time].height);

    speedText->position->setCoords(pnv_trace[avp_cur_time].lenght + 15, pnv_trace[avp_cur_time].height + 20);
    speedText->setText("V = " + QString::number(pnv_trace[avp_cur_time].v));
    heightText->position->setCoords(pnv_trace[avp_cur_time].lenght + 15, pnv_trace[avp_cur_time].height + 500);
    heightText->setText("H = " + QString::number(pnv_trace[avp_cur_time].height));

    avp_cur_time++;
    ui->avpPlot->replot();
}
void MainWindow::show_area_info() {
    areaOutputBox->setRowCount(1);	// Очищаем прежний результат
    areaOutputBox->setRowCount(1+pnvInfo.areas.size());	// Добавляем место под новый

    for(int i = 0; i < int(pnvInfo.areas.size()); ++i) {
        if(pnvInfo.areas[i].type == PNVArea::NoneType) continue;

        areaOutputBox->setItem(i+1, 0, new QTableWidgetItem(QString::number(i+1) ) );
        areaOutputBox->setItem(i+1, 1, new QTableWidgetItem(QString::number(pnvInfo.areas[i].h1) ) );
        areaOutputBox->setItem(i+1, 2, new QTableWidgetItem(QString::number(pnvInfo.areas[i].h2) ) );
        areaOutputBox->setItem(i+1, 3, new QTableWidgetItem(QString::number(pnvInfo.areas[i].v1) ) );
        areaOutputBox->setItem(i+1, 4, new QTableWidgetItem(QString::number(pnvInfo.areas[i].v2) ) );
        areaOutputBox->setItem(i+1, 5, new QTableWidgetItem(QString::number(pnvInfo.areas[i].vc) ) );
        areaOutputBox->setItem(i+1, 6, new QTableWidgetItem(QString::number(pnvInfo.areas[i].vy) ) );
    }

    areaOutputBox->resizeRowsToContents();
    setActive(ui->area_out_btn, "notice");
}


/* Для переноса результатов */
void MainWindow::copyOurInClipboard() {
    QString points;
    for_each(our_model_track.begin(), our_model_track.end(), [&points](Model::motionParamLA point) {
        points.append(QString::number(point.X) + " " + QString::number(point.Y) + " " + QString::number(point.Z) + "  " + QString::number(point.psi) + "\n");
    });

    if(QClipboard* clipboard = QApplication::clipboard())
        clipboard->setText(points);

}
void MainWindow::copyTargetInClipboard() {
    QString points;
    for_each(target_model_track.begin(), target_model_track.end(), [&points](Model::motionParamLA point) {
         points.append(QString::number(point.X) + " " + QString::number(point.Y) + " " + QString::number(point.Z) + " " + QString::number(point.psi) + "\n");
    });

    if(QClipboard* clipboard = QApplication::clipboard())
        clipboard->setText(points);
}
void MainWindow::copyAircraftInClipboard() {

    auto s = [](double number) { return QString::number(number) + " ";};
    QString points;

    size_t size = std::min(our_model_track.size(), target_model_track.size());
    for(size_t i = 0; i < size; ++i) {
        Model::motionParamLA p = our_model_track[i], t = target_model_track[i];
        points.append(
                        s(i) + s(p.X) + s(p.Y) + s(p.Z) + s(p.v) + s(p.psi) + s(p.theta)
                      + s(t.X) + s(t.Y) + s(t.Z) + s(t.v) + s(t.psi) + s(t.theta)
                    );
    }

    if(QClipboard* clipboard = QApplication::clipboard())
        clipboard->setText(points);
}
void MainWindow::buildInterceptTrajectory() {
    // Проверка что ПНВ и Участки загружены, если нет переводит на окно загрузки
    //if(!readBox->isAreaLoad() || !readBox->isPnvLoad()) {
    //    QMessageBox::warning(this, QString::fromLocal8Bit("Укажите данные"), QString::fromLocal8Bit("Загрузите Участки и ПНВ"));
    //    removePreviousWidget();
    //    setActiveBox(readBox, ui->data_in_layout);
    //    setActive(previousClickInBtn);
    //    setActive(ui->read_in_btn, "active");
    //    previousClickInBtn = ui->read_in_btn;
    //    return;
    //}

   our_model_track.clear();
   target_model_track.clear();

   LA a1, a2;
   posBox->inputs(a1, a2);
   Model::motionParamLA our(a1.x, a1.y, a1.z, a1.v, a1.psi, a1.theta, 0.0);
   Model::motionParamLA target(a2.x, a2.y, a2.z, a2.v, a2.psi, a2.theta, 0.0);

   Model::PointingMathModel::Config model_config;
   MethodType method;
   angpBox->angpInput(model_config.dl, model_config.q_zad, model_config.gamma1, model_config.gamma2, method);
   switch (method) {
       case MethodType::MANEVR: model_config.method = Model::Method::MANEVR; break;
       case MethodType::PEREHVAT: model_config.method = Model::Method::INTERCEPTION; break;
       case MethodType::PEREH_MANEVR: model_config.method = Model::Method::TURN_INTERCEPTION; break;
       default: model_config.method = Model::Method::MANEVR;
   }
   model_config.find_optimal_manevr = angpBox->is_optimal();

   QString area_filepath_choose, pnv_filepath_choose;
   if(readBox->isAreaLoad() && readBox->isPnvLoad())
       readBox->getFilePaths(area_filepath_choose, pnv_filepath_choose);

   settings->beginGroup("Paths");
   QString area_filepath = readBox->isAreaLoad() ? area_filepath_choose : settings->value("pnv_areas").toString();
   QString pnv_filepath =  readBox->isPnvLoad() ? pnv_filepath_choose : settings->value("pnv_paths").toString();
   settings->endGroup();
   Reader::read_model_area(area_filepath, climbProgramConsts);
   Reader::read_model_pnv(pnv_filepath, climbProgramConsts);

   double vn1, vn2, vn3, vn4;
   avpBox->getProgramSpeed(vn1, vn2, vn3, vn4);
   climbProgramConsts.setVN(vn1, vn2, vn3, vn4);


   Model::PointingMathModel model(climbProgramConsts, model_config);

   double timeDefeat;
   bool result = model.pointingDefeatAircraftOnBase(our, target, timeDefeat);
   //bool result = model.pointingDefeatAircraftAzimuth(our, target);
   //bool result = model.test(our, target);

   if(!result) {
       QMessageBox::warning(this, QString::fromLocal8Bit("Наведение невозможно"),
                            QString::fromLocal8Bit("С данными параметрами не удалось построить траекторию перехвата"));
       return;
   }


   model.extrapolate(our_model_track);
   model.extrapolateTarget(target, our_model_track.size(), target_model_track);

   if(this->findChild<QCheckBox*>("AutoStartVisual")->isChecked())
       this->findChild<QToolButton*>("TrackVisualBtn")->click();

}
