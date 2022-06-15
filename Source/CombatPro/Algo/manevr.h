#pragma once

#include "Algo\combat.h" // для структур 

#include <vector>
#include <cmath>

using std::vector;


struct InterceptionInfo {
	bool possible;			// Признак возможности наведения метдом перехват
	double fi;				// Необходимый курс наведения
	double time;			// Необходиме время движение по курсу fi
	double dist;			// Расчетная дистанция пути по fi за время time

	InterceptionInfo() { }
	InterceptionInfo(bool _p, double _fi, double _time, double _dist) :
	possible(_p), fi(_fi), time(_time), dist(_dist) { }

    bool is_possible()  const { return possible; }
    double getCource()  const { return fi; }
    double getTime()    const { return time; }
    double getDist()    const { return dist; }
};

struct PursuitInfo {
    bool possible;			// Признак возможности наведения метдом перехват
    double time;			// Необходиме время движение до заданной точки
    double dist;			// Пройденный путь

    PursuitInfo() : possible(false), time(-1.0), dist(0.0) { }
    PursuitInfo(bool _p, double _time, double _dist)
        : possible(_p), time(_time), dist(_dist) { }


};

struct ManeuverInfo {
private:
	coordinate center;			// Координат цента окружности виража
	double R;					// Радиус окружности виража
	double angle_in;			// Угол точки входа в окружности
	double angle_out;			// Угол точки выхода в окружности
    coordinate outPoint;        // Координаты точки выхода с траектории маневра
    double need_time;			// Необходимое время выполнения виража
	double omega;				// Угловая скорось выполення виража
								// Если omega > 0, то движение 0...360 (против часовой), иначе 360...0 (по часовой)

	bool possible;				// Признак возможности совершения маневра

public:
	ManeuverInfo() { }
    ManeuverInfo(coordinate _center, double r, double t, double in, double out, double _omega) :
        center(_center), R(r), angle_in(in), angle_out(out), need_time(t), omega(_omega) { }

    coordinate getCenter()      const { return center; }
	coordinate getInPoint()		const {
		double x, y, z;
		z = center.z + R*cos(angle_in);
		x = center.x + R*sin(angle_in);
		y = center.y;

		return coordinate(x, y, z);
	}
    coordinate getOutPoint() const {
		double x, y, z;

		z = center.z + R*cos(angle_out);
		x = center.x + R*sin(angle_out);
		y = center.y;

		return coordinate(x, y, z);
	}
    double getInAngle()         const { return angle_in; }
    double getOutAngle()       const { return angle_out; }
    double getRadius()          const { return R; }
    double getNeedTime()        const { return need_time; }
    double getAngularVelocity() const { return omega; }

    bool is_possible()          const { return possible; }
};

struct TwoManeuverInfo {
	ManeuverInfo minfo1;		// Информация о начальном развороте
	double kurs1;				// Курс прямолинейного участка (между начальным разворотом и конечным) [рад]
	double lenght;				// Длина прямолинейного участка
	ManeuverInfo minfo2;		// Информация о развороте для выхода на цель под заданным курсом
	double attack_kurs;			// Курс выхода на цель
	double time;				// Время общего пути
	bool possible;				// Признак возможности выполнения

	TwoManeuverInfo(ManeuverInfo minfo1, double kurs, double lenght, ManeuverInfo minfo2, double attack_kurs, double time, bool possible) {
		this->minfo1 = minfo1;
		this->kurs1 = kurs;
		this->lenght = lenght;
		this->minfo2 = minfo2;
		this->time = time;
		this->possible = possible;
	}
	TwoManeuverInfo() { }
};

// Метод наведения параллельное сближение
InterceptionInfo interception(const LA& our, const LA& target);

// Метод наведения прямой перехват
InterceptionInfo interception_dl(const LA& our, const LA& target, double dl);

// Метод наведения погоня
PursuitInfo pursuit_dl(const LA& our, const LA& target, double dl);

// Метод наведения модифицированный прямой перехват
int interception_maneuver(const LA& our, const LA& target, double dl, double gamma, ManeuverInfo& minfo, InterceptionInfo& info);

// Метод наведения маневр с двумя разворотами
TwoManeuverInfo twoManeuver2(const LA& our, const LA& target, double gamma1, double gamma2, double q_zad, double dl);
ManeuverInfo buildManevr(double v1, double fi1, double v2, double fi2, double gamma, coordinate p1, coordinate p2);
TwoManeuverInfo optimal_manevr(const LA& our, const LA& target, double gamma1, double gamma2, double q_zad, double dl);


// Расчет информации для маневра
ManeuverInfo buildManevrInfo(const LA& pos, double fi, double gamma);


// Траектория раворота
void buildManevrTrack(const LA& pos, const ManeuverInfo& minfo, vector<LA>& track);

// Траектория перехвата
void buildInterceptionTrack(const LA& our, InterceptionInfo info, vector<TrackPoint>& trajectory, int steps = -1);

// Траектория разворота
void buildManevrTrack2(coordinate startPoint, ManeuverInfo minfo, vector<TrackPoint>& trajectory);

// Траектория разворот-перехват
void buildTrack2(const LA& our, const ManeuverInfo& minfo, const InterceptionInfo& info, vector<TrackPoint>& track);

// Траектория погони
void buildPogonyaTrack(const LA& our, const LA& target, double time, vector<TrackPoint>& track, int steps = -1);

// Траектория маневра
void buildTwoManeuvertrack(const LA& our, const LA& target, const TwoManeuverInfo& m2info, vector<TrackPoint>& track);

// Прямолинейная Траектория цели
void buildTargetTrack(const LA& target, double time, vector<TrackPoint>& track);

