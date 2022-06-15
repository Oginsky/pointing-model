#pragma once

#include "Algo\combat.h" // ��� �������� 

#include <vector>
#include <cmath>

using std::vector;


struct InterceptionInfo {
	bool possible;			// ������� ����������� ��������� ������ ��������
	double fi;				// ����������� ���� ���������
	double time;			// ���������� ����� �������� �� ����� fi
	double dist;			// ��������� ��������� ���� �� fi �� ����� time

	InterceptionInfo() { }
	InterceptionInfo(bool _p, double _fi, double _time, double _dist) :
	possible(_p), fi(_fi), time(_time), dist(_dist) { }

    bool is_possible()  const { return possible; }
    double getCource()  const { return fi; }
    double getTime()    const { return time; }
    double getDist()    const { return dist; }
};

struct PursuitInfo {
    bool possible;			// ������� ����������� ��������� ������ ��������
    double time;			// ���������� ����� �������� �� �������� �����
    double dist;			// ���������� ����

    PursuitInfo() : possible(false), time(-1.0), dist(0.0) { }
    PursuitInfo(bool _p, double _time, double _dist)
        : possible(_p), time(_time), dist(_dist) { }


};

struct ManeuverInfo {
private:
	coordinate center;			// ��������� ����� ���������� ������
	double R;					// ������ ���������� ������
	double angle_in;			// ���� ����� ����� � ����������
	double angle_out;			// ���� ����� ������ � ����������
    coordinate outPoint;        // ���������� ����� ������ � ���������� �������
    double need_time;			// ����������� ����� ���������� ������
	double omega;				// ������� ������� ��������� ������
								// ���� omega > 0, �� �������� 0...360 (������ �������), ����� 360...0 (�� �������)

	bool possible;				// ������� ����������� ���������� �������

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
	ManeuverInfo minfo1;		// ���������� � ��������� ���������
	double kurs1;				// ���� �������������� ������� (����� ��������� ���������� � ��������) [���]
	double lenght;				// ����� �������������� �������
	ManeuverInfo minfo2;		// ���������� � ��������� ��� ������ �� ���� ��� �������� ������
	double attack_kurs;			// ���� ������ �� ����
	double time;				// ����� ������ ����
	bool possible;				// ������� ����������� ����������

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

// ����� ��������� ������������ ���������
InterceptionInfo interception(const LA& our, const LA& target);

// ����� ��������� ������ ��������
InterceptionInfo interception_dl(const LA& our, const LA& target, double dl);

// ����� ��������� ������
PursuitInfo pursuit_dl(const LA& our, const LA& target, double dl);

// ����� ��������� ���������������� ������ ��������
int interception_maneuver(const LA& our, const LA& target, double dl, double gamma, ManeuverInfo& minfo, InterceptionInfo& info);

// ����� ��������� ������ � ����� �����������
TwoManeuverInfo twoManeuver2(const LA& our, const LA& target, double gamma1, double gamma2, double q_zad, double dl);
ManeuverInfo buildManevr(double v1, double fi1, double v2, double fi2, double gamma, coordinate p1, coordinate p2);
TwoManeuverInfo optimal_manevr(const LA& our, const LA& target, double gamma1, double gamma2, double q_zad, double dl);


// ������ ���������� ��� �������
ManeuverInfo buildManevrInfo(const LA& pos, double fi, double gamma);


// ���������� ��������
void buildManevrTrack(const LA& pos, const ManeuverInfo& minfo, vector<LA>& track);

// ���������� ���������
void buildInterceptionTrack(const LA& our, InterceptionInfo info, vector<TrackPoint>& trajectory, int steps = -1);

// ���������� ���������
void buildManevrTrack2(coordinate startPoint, ManeuverInfo minfo, vector<TrackPoint>& trajectory);

// ���������� ��������-��������
void buildTrack2(const LA& our, const ManeuverInfo& minfo, const InterceptionInfo& info, vector<TrackPoint>& track);

// ���������� ������
void buildPogonyaTrack(const LA& our, const LA& target, double time, vector<TrackPoint>& track, int steps = -1);

// ���������� �������
void buildTwoManeuvertrack(const LA& our, const LA& target, const TwoManeuverInfo& m2info, vector<TrackPoint>& track);

// ������������� ���������� ����
void buildTargetTrack(const LA& target, double time, vector<TrackPoint>& track);

