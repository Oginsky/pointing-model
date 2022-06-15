#ifndef MATFUNCTION_H
#define MATFUNCTION_H

#include "math.h"

void ugl(double ca,double cb,double cg, double &eg,double &ev) {
double ed=0.99999;
double q;

   if (cb >= ed) { eg = 0.0;     ev = M_PI / 2;   return; }
   if (cb <=-ed) { eg = 0.0;     ev =-M_PI / 2;   return; }
   if (ca >= ed) { eg = 0.0;     ev = 0.0;      return; }
   if (ca <=-ed) { eg =  M_PI;     ev = 0.0;      return; }
   if (cg >= ed) { eg = M_PI/2;    ev = 0.0;      return; }
   if (cb <=-ed) { eg = 3*M_PI/2;  ev = 0.0;      return; }

   ev = M_PI/2 - acos(cb);
   q = ca / cos(ev);
   if (q>1)  q= 1;
   if (q<-1) q=-1;
   eg = acos(q);
   if (cg < 0.0) eg = 2.0*M_PI - eg;

}


void par(double x1,double y1,double z1,double x2,double y2,double z2,double psi1,double psi2,
         double v1,double v2,double tttv1,double tttv2,
         double &d21,double &ev,double &eg,double &dt21,double &fv,double &fg,double &qr2,double &v2r,
         double &xt1,double &yt1,double &zt1,double &xt2,double &yt2,double &zt2,
         int &ipps,
         double &vx1l,double &vy1l,double &vz1l,double &vx2l,double &vy2l,double &vz2l,double &wlb)
{
double x21,y21,z21,xt21,yt21,zt21,calv,cblv,cglv,wlby,wlbz;

   x21  =  x2 - x1;
   y21  =  y2 - y1;
   z21  =  z2 - z1;

   d21  =  sqrt(x21*x21  +  y21*y21  +   z21*z21);

   xt1  =  v1 * cos( tttv1 )*cos(psi1);
   yt1  =  v1 * sin( tttv1 );
   zt1  =  v1 * cos( tttv1 )*sin(psi1);

   xt2  =  v2 * cos( tttv2 )*cos(psi2);
   yt2  =  v2 * sin( tttv2 );
   zt2  =  v2 * cos( tttv2 )*sin(psi2);

   xt21 =  xt2 - xt1;
   yt21 =  yt2 - yt1;
   zt21 =  zt2 - zt1;

   if (d21 == 0)
   {
     calv =  0;
     cblv =  0;
     cglv =  0;
   }
   else
   {
     calv =  x21/d21;
     cblv =  y21/d21;
     cglv =  z21/d21;
   }

   ugl( calv, cblv, cglv, eg,  ev );
   if  (eg > 2*M_PI)  eg  =  eg - 2.0*M_PI;

   vx1l =  xt1 * cos(ev)* cos(eg) + yt1*sin(ev) + zt1*cos(ev)*sin(eg);
   vy1l = -xt1 * sin(ev)* cos(eg) + yt1*cos(ev) - zt1*sin(ev)*sin(eg);
   vz1l = -xt1 * sin(eg)   +   zt1*cos(eg);

   vy2l = -xt2 * sin(ev)* cos(eg) + yt2*cos(ev) - zt2*sin(ev)*sin(eg);
   vx2l =  xt2 * cos(ev)* cos(eg) + yt2*sin(ev) + zt2*cos(ev)*sin(eg);
   vz2l = -xt2 * sin(eg)   +   zt2*cos(eg);

   fg   =  eg - psi1;

   fv   =  ev - tttv1;

   qr2  =  M_PI - psi2 + eg;
   if  (eg>=psi2)  qr2 = M_PI - eg + psi2;

   ipps = 0;
   if  (vx2l <= 0) ipps = 1;

   v2r  = -vx2l;

   dt21 = vx2l - vx1l;
   if (d21 == 0)
   { wlby= 0;  wlbz= 0; wlb= 0; }
   else
   {
     wlby = ( vy2l -  vy1l )/d21;
     wlbz = ( vz2l -  vz1l )/d21;
     wlb  = sqrt(wlby*wlby  +  wlbz*wlbz);
   }
}


#endif // MATFUNCTION_H
