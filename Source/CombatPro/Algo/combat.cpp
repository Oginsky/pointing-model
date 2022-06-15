#include "Algo/combat.h"

#include <cmath>
#include <limits>

using std::abs;


double angleBetweenPoints(double cx1, double cy1, double cx2, double cy2) {
	double dist = sqrt((cx2 - cx1)*(cx2 - cx1) + (cy2 - cy1)*(cy2 - cy1));
	return asin( (cy2 - cy1) / dist );
}
double angleBetweenPoints(Point2D center, Point2D point) {
	double x = point.x - center.x, y = point.y - center.y;
	if(x == 0.0) return ( (y > 0.0) ? GPI2 : 3*GPI2 );
	double theta(atan(y/x));
	if(x > 0.0) return ( (y >= 0.0) ? theta: 2*GPI + theta );
	return GPI + theta;
}

inline void pointInLocCoordSys(const coordinate& center, const coordinate& point, double& x21, double& y21, double& z21) {
	x21 = point.x - center.x,
	y21 = point.y - center.y,
	z21 = point.z - center.z;
}

inline coordinate pointInLocCoordSys(const coordinate& center, const coordinate& point) {
	double x21 = point.x - center.x,
		   y21 = point.y - center.y,
		   z21 = point.z - center.z;
	return coordinate(x21, y21, z21);
}

double distance(double dx, double dy, double dz) { return sqrt(dx*dx + dy*dy + dz*dz); }

inline double distancePoint(const coordinate& point1, const coordinate& point2) {
	coordinate dist = pointInLocCoordSys(point1, point2);
	return sqrt(dist.x*dist.x + dist.y*dist.y + dist.z*dist.z);
}

inline void getDerivativesCoordinates(const LA& la, double& xt, double& yt, double& zt) {
	xt  =  la.v * cos( la.theta )*cos(la.psi);
	yt  =  la.v * sin( la.theta );
    zt  =  la.v * cos( la.theta )*sin(la.psi);
}

inline void projVelocityVecToAntennaAxis(
	double xt, double yt, double zt, double eg, double ev,
	double& vxl, double& vyl, double& vzl
)
{
	double cev(cos(ev)), sev(sin(ev)), ceg(cos(eg)), seg(sin(eg));

	vxl =  xt*cev*ceg	+ yt*sev	+ zt*cev*seg;
	vyl = -xt*sev* ceg	+ yt*cev	- zt*sev*seg;
	vzl = -xt*seg					+ zt*ceg;
}

void ugl(double ca,double cb,double cg, double &eg,double &ev) {

    double ed = 0.99999;
    double q;

   if (cb >= ed) { eg = 0.0;     ev = GPI2;   return; }
   if (cb <=-ed) { eg = 0.0;     ev =-GPI2;   return; }
   if (ca >= ed) { eg = 0.0;     ev = 0.0;        return; }
   if (ca <=-ed) { eg =  GPI;     ev = 0.0;      return; }
   if (cg >= ed) { eg = GPI2;    ev = 0.0;      return; }
   if (cb <=-ed) { eg = 3*GPI2;  ev = 0.0;      return; }

   ev = GPI2 - acos(cb);
   q = ca / cos(ev);
   if (q>1)  q= 1;
   if (q<-1) q=-1;
   eg = acos(q);
   // if (cg <= 0.0 && q < 0) eg = 2.0*GPI - eg;
   if (cg < 0.0) eg = 2.0*GPI - eg;
}


ParametersSet<double> par(const LA& our, const LA& target) {
	double x21, y21, z21;
	pointInLocCoordSys(our, target, x21, y21, z21);
	double d21 = distance(x21, y21, z21);

	double xt1, yt1, zt1;
	getDerivativesCoordinates(our, xt1, yt1, zt1);
	double xt2, yt2, zt2;
	getDerivativesCoordinates(target, xt2, yt2, zt2);

	double eg, ev;
	(d21 == 0) ? ugl(0, 0, 0, eg, ev) : ugl(x21/d21, y21/d21, z21/d21, eg, ev);
	if  (eg > 2*GPI)  eg  =  eg - 2.0*GPI;

	double vx1l, vy1l, vz1l;
	projVelocityVecToAntennaAxis(xt1, yt1, zt1, eg, ev, vx1l, vy1l, vz1l);
	double vx2l, vy2l, vz2l;
	projVelocityVecToAntennaAxis(xt2, yt2, zt2, eg, ev, vx2l, vy2l, vz2l);

	double fg   =  eg - our.psi, fv   =  ev - our.theta;
	double qr2  =  GPI - target.psi + eg;
	//if(eg >= target.psi)  qr2 = GPI - eg + target.psi;
	if(eg >= target.psi)  qr2 -= 2*GPI*sign(qr2);

	double qr2_vert = ev - target.theta;                    // вертикальный ракурс цели, [рад.]
	if (abs(qr2_vert) > GPI) qr2_vert = qr2_vert - 2 * GPI * sign(fv);

   double ipps = 0; // создать enum для признак ППС или ЗПС 
   if  (vx2l <= 0) ipps = 1;

   double v2r  = -vx2l;
   double dt21 = vx2l - vx1l;

   double wlby, wlbz, wlb;
   if (d21 == 0){ wlby = wlbz = wlb = 0; }
   else
   {
     wlby = ( vy2l -  vy1l )/d21;
     wlbz = ( vz2l -  vz1l )/d21;
     wlb  = sqrt(wlby*wlby  +  wlbz*wlbz);
   }

	ParametersSet<double> out;
	out["d21"] = d21, out["xt1"] = xt1, out["yt1"] = yt1, out["zt1"] = zt1,
	out["xt2"] = xt2, out["yt2"] = yt2, out["zt2"] = zt2,
	out["vx1l"] = vx1l, out["vy1l"] = vy1l, out["vz1l"] = vz1l, out["vx2l"] = vx2l, out["vy2l"] = vy2l, out["vz2l"] = vz2l,
	out["ev"] = ev, out["eg"] = eg, out["fg"] = fg, out["fv"] = fv, out["qr2"] = qr2, out["qr2_v"] = qr2_vert,
	out["v2r"] = v2r, out["ipps"] = ipps,
	out["wlb"] = wlb, out["dt21"] = dt21;
	return out;
}
