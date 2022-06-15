#ifndef POINTINGMODEL_H
#define POINTINGMODEL_H

#include <cmath>
#include <vector>

#include <QVector>

#include <QDebug>


using std::vector;

namespace Model {

static const double G(9.81);
static const double GPI(3.14159265358979323846);
static const double GPI2(GPI * 0.5);
const double G2PI(GPI*2.0);		// 2.0*пи
const double GPI3_2(GPI*1.5);		// 3*пи/2.0
const double GPI4(GPI*0.25);		// пи/4.0
const double GPI18(GPI4/18.0);	// пи/18.0

static const double d2r(GPI/180.0);
static const double r2d(180.0/GPI);

static const int km_c2m_c = 1.0/1000.0;

double angleBetweenPoints(double x0, double y0, double x1, double y1);
double distance(double dx, double dy, double dz);
double convertTo2PI(double angle);
double fromCourceToxAxisAngle(double cource);
double fromxAxisAngleToCource(double angle);
double section(double startAngle, double currentAngle, bool omega);
double getManevrTime(double inAngle, double outAngle, double omega);



/* МЕТОДЫ И УЧАСТКИ */
enum AreaType { line, manevr, uparea, downarea, fastarea, slowarea, balance };
enum Method { INTERCEPTION, MANEVR, PURSUIT, TURN_INTERCEPTION };

struct coordinates  // из g_geom.h
{
    double X, Y, Z;

    coordinates() : X(0.0), Y(0.0), Z(0.0) {  };
    coordinates(double x, double y, double z) : X(x), Y(y), Z(z) {  };

    double distance(const coordinates& point) const {
        double dx = X - point.X,
               dy = Y - point.Y,
               dz = Z - point.Z;

        return sqrt(dx*dx + dy*dy + dz*dz);
    }
};
struct motionParam : public coordinates	// Координаты и производные из g_geom.h
{
    double vX, vY, vZ;
    motionParam() : coordinates(), vX(0.0), vY(0.0), vZ(0.0) {};
    //(double vp, double qp, double ep): V(vp), Q(qp), E(ep) {};
    //(double xp, double yp, double zp, double vp, double qp, double ep) : coordinates(xp, yp, zp), V(vp), Q(qp), E(ep) {};
    double speed() const;							// Модуль скорости
    //void lineExtrapolation(double tmp, double tcur, bool prV=true);	// Линейная интерполяция параметров движения на момент t (prV - интерполяция производится по VQE, иначе по vXvYvZ)
    void extrapolation(double dt);

    unsigned int conversionTopoCartesToGeoCartes(coordinates &cs);	// Преобразование координат из ТЦ в ГЦ (cs-центр ТЦ системы)	// Преобразование координат из ТЦ в ГЦ (cs-центр ТЦ системы)
    motionParam(double xp, double yp, double zp, double vx, double vy, double vz) : coordinates(xp, yp, zp), vX(vx), vY(vy), vZ(vz) {}
};
struct motionParamLA : public motionParam {
    double psi;
    double theta;
    double t;
    double v;

    motionParamLA(double x, double y, double z, double _v, double _psi, double _theta, double _t)
        : motionParam(x, y, z, 0.0, 0.0, 0.0), psi(_psi), theta(_theta), v(_v), t(_t) {
        vX = v*sin(GPI2 - theta)*sin(GPI2 - psi);
        vZ = v*sin(GPI2 - theta)*cos(GPI2 - psi);
        vY = v*cos(GPI2 - theta);
    }
    motionParamLA(motionParam mov, double _v, double _psi, double _theta, double _t)
        : motionParam(mov), psi(_psi), theta(_theta), v(_v), t(_t) {
        vX = v*sin(GPI2 - theta)*sin(GPI2 - psi);
        vZ = v*sin(GPI2 - theta)*cos(GPI2 - psi);
        vY = v*cos(GPI2 - theta);
    }

    motionParamLA() : motionParam() { }
};

/* ПРЕДСТАВЛЕНИЕ БАЗОВЫХ ХАРАКТЕРИСТИК АК И ПНВ */
struct ClimbSectionConsts {
    double h1, h2;			// Начальная и конечная высота на участке (может совпадать)
    double v1, v2;			// Начальная и конечная скорость на участке (может совпадать)
    double vc;				// Ускорение на участке
    double vy;				// Вертикальная составляющая скорости

    /* Расчитываемые характеристики */
    double time;			// Время выполнения данного участка
    double lenght;			// Длина пути данного участка

    AreaType type;					// { Набор, снижение, ускорение, торможение }


    ClimbSectionConsts();
};

AreaType getPnvAreaType(unsigned type);

typedef vector<ClimbSectionConsts*> PnvPath;

struct ClimbProgramConsts {
    double VN1, VN2, VN3, VN4;		// Программные скорости
    double maxH;

    vector<ClimbSectionConsts> areas;			// Информация о стандартных участках
    vector<PnvPath> pnvPath;			// Информация о 12 ПНВ (последовательности участков из areas)
    vector<double> pnvTime;        // Время выполнения ПНВ без учета первого и балансового участка

    ClimbProgramConsts();
    void createPath(unsigned number_pnv, QVector<unsigned> areaNumbers);
    void setVN(double vn1, double vn2, double vn3, double vn4);
    unsigned getAreaNumber(const ClimbSectionConsts* area) const;
};


/* ПРЕДСТАВЛЕНИЕ ТРАЕКТОРИИ В МОДЕЛЕ */
typedef vector<motionParamLA> Track;

struct Area {
    AreaType type;

    double start_time;
    double time;
    double lenght;
    double fuel;

    double v0, h0, theta0, kurs0;

    bool possible;
    bool calc;

    coordinates start_point;
    coordinates end_point;

    Area(AreaType _type, coordinates sp, double _time = -1, double _lenght = -1, double _fuel = -1);
    Area(AreaType _type, motionParamLA pos);

    void setPosition(const motionParamLA& pos);

    virtual void integrate(motionParamLA& pos) = 0;
    virtual void integrate_pos(motionParamLA& pos, double t = -1.0) = 0;
    virtual void extrapolate(Track& track, double t = -1, double dt = 1.0) = 0;
    virtual motionParamLA position() = 0;

};
struct PnvArea : public Area {
    double v2;
    double h2;
    double vc, vy;

    PnvArea(AreaType _type, coordinates sp);
    PnvArea(AreaType _type, motionParamLA pos);

    virtual void integrate(motionParamLA& pos) override;
    virtual void integrate_pos(motionParamLA& pos, double t = -1.0) override;
    virtual void extrapolate(Track& track, double t = -1, double dt = 1.0) override;
    virtual motionParamLA position() override;

};
struct HorizontalLine : public Area {

    HorizontalLine(coordinates sp);
    HorizontalLine();

    virtual void integrate(motionParamLA& pos) override;
    virtual void integrate_pos(motionParamLA& pos, double t = -1.0) override;
    virtual void extrapolate(Track& track, double t = -1, double dt = 1.0) override;
    virtual motionParamLA position() override ;
};
struct TurnManevr : public Area {
    coordinates center;			// Координат цента окружности виража
    double radius;				// Радиус окружности виража
    double angle_in;			// Угол точки входа в окружности
    double angle_out;			// Угол точки выхода в окружности
    double omega;				// Угловая скорось выполення виража
                                // Если omega > 0, то движение 0...360 (против часовой), иначе 360...0 (по часовой)
    double alpha;               // Размер проходимой секции [рад]
    double gamma;               // Угол крена и направление разворота (gamma > 0) - по часовой

    TurnManevr(coordinates sp);
    TurnManevr(motionParamLA position);
    TurnManevr();

    double getKursByAngle();
    static double count_radius(double v, double gamma);
    void clear();

    virtual void integrate(motionParamLA& pos) override;
    virtual void integrate_pos(motionParamLA& pos, double t = -1.0) override;
    virtual void extrapolate(Track& track, double t = -1, double dt = 1.0) override;
    virtual motionParamLA position() override;
};

/* МОДЕЛЬ */



class PointingMathModel {

public:
    struct Config {
        Method method;				// Выбранный метод
        double dt;					// Шаг по времени (для набора точек траектории)
        double gamma1, gamma2;		// Углы крена первого и второго развората (отрицательное - разворот почасовой, положительое - против часовой)
        double dl;					// Дистанция до цели в конце наведения
        double q_zad;				// Угол выхода на цель в конце наведения
        double radius;				// Радиус разворота в методе манёвр

        bool is_input_radius;		// Признак заданного радиуса разворота
        bool find_optimal_meth;		// Признак перебора методов наведения и выбор лучшего из них
        bool find_optimal_manevr;	// Признак перебора возможных разворотов и выбор лучшего варианта

        Config() {
            dt = 1.0;
            gamma1 = gamma2 = 45.0*d2r;
            q_zad = 179.0*d2r;
            dl = 1000.0;
            method = Method::MANEVR;

            is_input_radius		= false;
            find_optimal_meth	= false;
            find_optimal_manevr = true;
        }

    };


protected:
    Config config;						// Входные параметров методов наведения
    ClimbProgramConsts pnvInfo;                    // Базовые характеристики

    /* ПНВ */
    vector<PnvArea*> pnv_areas;
    double pnv_time, pnv_length, pnv_average_v, program_v;

    motionParamLA our_start, target_start;     // Начальные данные о положении

    double firstup_time;            // Время до начала разворота
    double time;                    // Общее время наведения

    vector<Area*> areas;           // Параметры участков наведения

    /* Подготовительные метод */
    void pick_pnv(const motionParamLA& target, unsigned& pnv_number, unsigned& pnv_group);
    void prepare_pnv_sections(const motionParamLA& our, const motionParamLA& target, unsigned pnv_number, unsigned pnv_group);
    PnvArea* build_pnv_section(const motionParamLA& current_position, const motionParamLA& target, const ClimbSectionConsts& sectionConst);
    void count_pnv_characters();
    unsigned getNumberPnv(const motionParamLA& our, const motionParamLA& target);


    /* Методы наведения */
    bool two_turn_maneuver(const motionParamLA& our, const motionParamLA& target, TurnManevr* turn1, HorizontalLine* line, TurnManevr* turn2);
    bool interception(const motionParamLA& our, const motionParamLA& target, HorizontalLine* area);
    bool interception_dl(const motionParamLA& our, const motionParamLA& target, HorizontalLine* area);
    bool turn_interception_dl(const motionParamLA& our, const motionParamLA& target,TurnManevr* turn, HorizontalLine* line);
    bool pursuit();

    /* Методы корректировки */
    void turn_to_lead_point(const motionParamLA& our, const motionParamLA& target, double time, TurnManevr* area);
    void turn_to_lead_point_modern(const motionParamLA& our, const motionParamLA& target, double time, double length, size_t index_ection, TurnManevr*& area);
    void turn_to_lead_point_modern2(const motionParamLA& our, const motionParamLA& target, double time, double length, size_t index_ection, TurnManevr*& area);

    /* Вспомогательные методы */
    void maneuver_conversion_in(const motionParamLA& our, const motionParamLA& target, double& B, double& P, double& v1, double& v2, double& g0, int& change_m);
    void maneuver_conversion_out(long double& g1, long double& R, long double& C, double psi2);
    double maneuver_init_solution(double b, double p, double fip, double dl, double a1);
    bool optimal_maneuver(const motionParamLA& our, const motionParamLA& target, TurnManevr*& turn1, HorizontalLine*& line, TurnManevr*& turn2);
    bool optimal_turn(const motionParamLA& our, const motionParamLA& target,TurnManevr*& turn, HorizontalLine*& line);
    PnvArea* build_balance_section(const motionParamLA& cur_our, const motionParamLA& tar, double cur_time);

    /* Формирование подзадач */
    void make_turn_subtask(const motionParamLA& our, const motionParamLA& target, motionParamLA& subour, motionParamLA& subtarget);

    void clear_model();


public:
    PointingMathModel(Config&& _config = Config());
    PointingMathModel(const ClimbProgramConsts& pnvinfo, Config _config) : pnvInfo(pnvinfo), config(_config) { }

    /* Методы построения наведения */
    bool pointingDefeatAircraftOnBase(const motionParamLA& our, const motionParamLA& target, double& timeDefeat);     // истребителям с аэродромов
    void pointingDefeatAircraftInAir(const motionParamLA& our, const motionParamLA& target, double& timeDefeat);      // свободным истребителям
    void pointingDefeatAircraftPatrol(const motionParamLA& our, const motionParamLA& target, double& timeDefeat);     // истребителям, дежурящим в зоне баражирования

    bool pointingDefeatAircraftAzimuth(const motionParamLA& our, const motionParamLA& target);

    void buildArea(const motionParamLA& pos, PnvArea* area, const ClimbSectionConsts& charcter, const motionParamLA& target);

    /* Результаты расчетов */
    void extrapolate(Track& track);
    motionParamLA get_position(double t);
    motionParamLA get_first_position();

    double test(const motionParamLA& our, const motionParamLA& target);

    double get_start_time();
    double get_pointing_time();
    void set_model_info(const motionParamLA& our, const motionParamLA& target);
    bool need_repoint(const motionParamLA& our, const motionParamLA& target);

    /* Траектория цели */
    void integrateTarget(const motionParamLA& target, double time, motionParamLA& pos2);
    void extrapolateTarget(const motionParamLA& target, double time, Track& track);


};


}


#endif // POINTINGMODEL_H
