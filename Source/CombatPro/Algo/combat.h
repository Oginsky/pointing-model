#pragma once

#include "Tools/parametersset.h"


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

//enum Status {IMPOSSIBLE = -1};



struct Point2D {
	double x, y;

	Point2D() { }
	Point2D(double _x, double _y) : x(_x), y(_y) { }
};
struct coordinate {
	double x, y, z;

	coordinate() { }
	coordinate(double _x, double _y, double _z) : x(_x), y(_y), z(_z) { }
	coordinate(const coordinate& other) : x(other.x), y(other.y), z(other.z) { }
};
struct TrackPoint: public coordinate {
	double t;
	
	TrackPoint() { }
	TrackPoint(double _t, double _x, double _y, double _z) : coordinate(_x, _y, _z), t(_t) {} 
	TrackPoint(coordinate p, double _t) : coordinate(p), t(_t) {} 
};
struct AngleParam {
	double psi, theta; // курс, траектория

	AngleParam() { }
	AngleParam(double _psi, double _theta) : psi(_psi), theta(_theta) { }
};
struct MotionParam : public coordinate {
	double vx, vy, vz;
	double t, v;

	MotionParam() { }
	MotionParam(double x, double y, double z ,double _vx, double _vy, double _vz, double _t, double _v) :
		coordinate(x, y, z), vx(_vx), vy(_vy), vz(_vz), t(_t), v(_v) { }
};
struct LA : public MotionParam, AngleParam {
	LA() { }
	LA(MotionParam mp, AngleParam ap) : MotionParam(mp), AngleParam (ap) { }
    LA(double _t, double _x, double _y, double _z, double _v) {
            t = _t;
            x = _x; y = _y; z = _z;
            v = _v;
    }
};


template <typename T>
static T sign(const T& tmp) {
    return (tmp == T(0)) ? 0 :
        (tmp > T(0) ? 1 : -1);
}


double angleBetweenPoints(double cx1, double cy1, double cx2, double cy2);
double angleBetweenPoints(Point2D center, Point2D point);

double distance(double dx, double dy, double dz);
inline double distancePoint(const coordinate& point1, const coordinate& point2);

inline void pointInLocCoordSys(const coordinate& center, const coordinate& point, double& x21, double& y21, double& z21);
inline coordinate pointInLocCoordSys(const coordinate& center, const coordinate& point);

inline void getDerivativesCoordinates(const LA& la, double& xt, double& yt, double& zt);

inline void projVelocityVecToAntennaAxis(
    double xt, double yt, double zt, double eg, double ev,
    double& vxl, double& vyl, double& vzl
);

void ugl(double ca,double cb,double cg, double &eg,double &ev);

ParametersSet<double> par(const LA& our, const LA& target);

