#include "pnv.h"

#include "manevr.h"

#include <cmath>

using std::sin;
using std::cos;
using std::abs;


PNVArea::TYPE getPnvAreaType(unsigned type) {
	switch(type) {
		case 1: return PNVArea::TYPE::Climb;
		case 2: return PNVArea::TYPE::Cecline;
		case 3: return PNVArea::TYPE::Acceleration;
		case 4: return PNVArea::TYPE::Braking;
		default: return PNVArea::TYPE::NoneType;
	}
}

void PNVInfo::createPath(unsigned number_pnv, QVector<unsigned> areaNumbers) {
	if(pnvPath.size() <= number_pnv) pnvPath.resize(number_pnv);
	pnvPath[number_pnv-1].resize(areaNumbers.size());

	for(size_t i = 0; i < areaNumbers.size(); ++i) {
		if(areaNumbers[i] == 0) pnvPath[number_pnv-1][i] = nullptr;
		else pnvPath[number_pnv-1][i] = &areas[areaNumbers[i]-1];
	}
}

void PNVInfo::setVN(double vn1, double vn2, double vn3, double vn4) {
	VN1 = vn1; VN2 = vn2; VN3 = vn3; VN4 = vn4;
}

unsigned PNVInfo::getAreaNumber(const PNVArea* area) const {
	for(int i = 0; i < int(areas.size()); ++i) {
		if(area == &areas[i]) return (i+1);
	}

	return -1;
}


unsigned getNumberPnv(const PNVInfo& pnvInfo, const LA& our, const LA& target, FLY_MODE flying_mode) {
	unsigned pnv_group;
	if(target.v < pnvInfo.VN1) pnv_group = 1;
	else if(target.v < pnvInfo.VN2) pnv_group = 2;
	else if(target.v < pnvInfo.VN3) pnv_group = 3;
	else pnv_group = 4;

	return (pnv_group-1)*3 + (int)flying_mode + 1;		// 0...9 + 1...3 = 1...12
}

void getCoord(double x0, double z0, double v, double kurs, double theta, double& x1, double& z1) {
    x1 = x0 + v*sin(GPI2 - theta)*sin(GPI2 - kurs);
    z1 = z0 + v*sin(GPI2 - theta)*cos(GPI2 - kurs);
}

void integrateClimbArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track) {
	int napr = sign(v2 - cur_v); 
	double cur_t = track.back().time;
	double cur_l = track.back().lenght;
	double dt = pnvInfo.dt;
	//double cur_theta = pnvInfo.theta;
    double x, z;

	while(cur_h < h2) {
		if(abs(cur_v - v2) < abs( (cur_v + napr*vc) - v2 )) napr = 0;
		//if(h2 - cur_h < 500.0 && cur_theta > 3.0*d2r) cur_theta -= pnvInfo.default_dth; 

		//cur_h += cur_v * cos(GPI2 - pnvInfo.theta);
		cur_h += vy;
		cur_l += cur_v*dt + vc*dt*dt/2.0; 
		cur_v += napr*vc;
		cur_t += pnvInfo.dt;

        getCoord(track.back().x, track.back().z, cur_v, pnvInfo.kurs, pnvInfo.theta, x, z);
		track.push_back(PNVPoint(cur_t, cur_l, cur_h, cur_v));
        track.back().x = x, track.back().z = z;
	}
}
void integreateCeclineArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track) {
	int napr = sign(v2 - cur_v);
	double cur_t = track.back().time;
	double cur_l = track.back().lenght;
	double dt = pnvInfo.dt;
    double x, z;

	while(cur_h > h2) {
		if(abs(cur_v - v2) < abs( (cur_v + napr*vc) - v2 )) napr = 0;
		//cur_h -= cur_v * cos(GPI2 - pnvInfo.theta);
        cur_h += vy;
		cur_l += cur_v*dt + vc*dt*dt/2.0;
		cur_v += napr*vc;
		cur_t += pnvInfo.dt;

        getCoord(track.back().x, track.back().z, cur_v, pnvInfo.kurs, pnvInfo.theta, x, z);
		track.push_back(PNVPoint(cur_t, cur_l, cur_h, cur_v));
        track.back().x = x, track.back().z = z;
	}
}
void integrateAccelerationArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track) {
	int naprH = sign(h2 - cur_h);
	double cur_t = track.back().time;
	double cur_l = track.back().lenght;
	double dt = pnvInfo.dt;
    double x, z;

	while(cur_v < v2) {
		//double vy = cur_v * cos(GPI2 - pnvInfo.theta);
		if(abs(cur_h - h2) < abs( (cur_h + naprH*vy) - h2 )) naprH = 0;
		cur_h += naprH * vy;
		cur_l += cur_v*dt + vc*dt*dt/2.0;
		cur_v += vc;
		cur_t += pnvInfo.dt;

        getCoord(track.back().x, track.back().z, cur_v, pnvInfo.kurs, pnvInfo.theta, x, z);
		track.push_back(PNVPoint(cur_t, cur_l, cur_h, cur_v));
        track.back().x = x, track.back().z = z;
	}
}
void integrateBrakingArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track) {
	int naprH = sign(h2 - cur_h);
	double cur_t = track.back().time;
	double cur_l = track.back().lenght;
	double dt = pnvInfo.dt;
    double x, z;

	while(cur_v > v2) {
		//double vy = cur_v * cos(GPI2 - pnvInfo.theta);
		if(abs(cur_h - h2) < abs( (cur_h + naprH*vy) - h2 )) naprH = 0;
		cur_h += naprH * vy;
		cur_l += cur_v*dt + vc*dt*dt/2.0;
		cur_v -= vc;
		cur_t += pnvInfo.dt;

        getCoord(track.back().x, track.back().z, cur_v, pnvInfo.kurs, pnvInfo.theta, x, z);
		track.push_back(PNVPoint(cur_t, cur_l, cur_h, cur_v));
        track.back().x = x, track.back().z = z;
    }
}


vector<LA> targeting(const PNVPoint& pos, const LA& target, PNVInfo& pnvInfo) {
    vector<LA> manevrTrack;

    double dist = 1000.0 * pos.v;
    LA our(pos.time, pos.x, pos.height, pos.z, pos.v);
    our.psi = pnvInfo.kurs;

    //double zt = target.z + 1000.0 * target.v * cos(GPI2 - target.psi);
    //double xt = target.z + 1000.0 * target.v * sin(GPI2 - target.psi);
    //LA target2 = target;
    //target2.z = zt; target2.x = xt;
    InterceptionInfo info = interception_dl(our, target, -dist);
    pnvInfo.kurs = info.getCource();

    ManeuverInfo minfo1 = buildManevrInfo(our, info.getCource(), 30*d2r);
    ManeuverInfo minfo2 = buildManevrInfo(our, info.getCource(), -30*d2r);

    (minfo1.getNeedTime() < minfo2.getNeedTime()) ? buildManevrTrack(our, minfo1, manevrTrack)
                                                  : buildManevrTrack(our, minfo2, manevrTrack);

    return manevrTrack;
}
void trans(vector<LA>& manevrTrack, QVector<PNVPoint>& track) {
    double l = track.back().lenght;
    for(size_t i = 0; i < size_t(manevrTrack.size()); ++i) {
        LA p = manevrTrack[i];
        PNVPoint pp;
        pp.x = p.x;
        pp.height = p.y;
        pp.z = p.z;
        pp.v = p.v;

        l += p.v;
        pp.lenght = l;
        track.push_back(pp);

    }
}

static bool was_manevr = false;

void integrateArea(const PNVArea* area, PNVInfo& pnvInfo, double& cur_v, double& cur_h, double target_h, QVector<PNVPoint>& track, const LA& target) {
	double v2, h2, vc, vy;
	v2 = (area->v2 != -1.0) ? area->v2 : cur_v;
	vc = (area->vc != -1.0) ? area->vc : pnvInfo.default_vc;
	h2 = (area->h2 != -1.0) ? area->h2 : cur_h;
	vy = (area->vy != -1.0) ? area->vy : pnvInfo.default_vy;


    size_t index = track.size();
	switch (area->type) {
		case PNVArea::Climb:
            integrateClimbArea(cur_h, cur_v, h2, v2, vc, vy, pnvInfo, track);
			break;
        case PNVArea::Cecline:
            h2 = target_h;
            integreateCeclineArea(cur_h, cur_v, h2, v2, vc, vy, pnvInfo, track);
			break;
		case PNVArea::Acceleration:
        if(!was_manevr) {
                        vector<TrackPoint> target_track;
                        buildTargetTrack(target, track.back().time, target_track);
                        TrackPoint lp = target_track.back();
                        LA target2(lp.t, lp.x, lp.y, lp.z, target.v);
                        target2.psi = target.psi;

                        vector<LA> manevrTrack = targeting(track.back(), target2, pnvInfo);
                        trans(manevrTrack, track);
                        was_manevr = true;
                    }
            integrateAccelerationArea(cur_h, cur_v, h2, v2, vc, vy, pnvInfo, track);
			break;
		case PNVArea::Braking:
            integrateBrakingArea(cur_h, cur_v, h2, v2, vc, vy, pnvInfo, track);
			break;
	}

    // Для отображения по цвету участков
    for(index; index < track.size(); ++index)
        track[index].type = area->type;
}

/*
void integrateArea(const PNVArea* area, const PNVInfo& pnvInfo, double& cur_v, double& cur_h, QVector<PNVPoint>& track) {
	double v2, h2, vc, vy;
	v2 = (area->v2 != -1.0) ? area->v2 : cur_v;
	vc = (area->vc != -1.0) ? area->vc : pnvInfo.default_vc;
	h2 = (area->h2 != -1.0) ? area->h2 : cur_h;
	vy = (area->vy != -1.0) ? area->vy : pnvInfo.default_vy;

	size_t index = track.size();
	switch (area->type) {
		case PNVArea::Climb:
			integrateClimbArea(cur_h, cur_v, area->h2, v2, vc, vy, pnvInfo, track);
			break;
		case PNVArea::Cecline:
			integreateCeclineArea(cur_h, cur_v, area->h2, v2, vc, vy, pnvInfo, track);
			break;
		case PNVArea::Acceleration:
			integrateAccelerationArea(cur_h, cur_v, area->h2, v2, vc, vy, pnvInfo, track);
			break;
		case PNVArea::Braking:
			integrateBrakingArea(cur_h, cur_v, area->h2, v2, vc, vy, pnvInfo, track);
			break;
	}

		// Для отображения по цвету участков
	for(index; index < track.size(); ++index)
		track[index].type = area->type;
}
*/
void buildPnvTrack(const LA& our, const LA& target, PNVInfo& pnvInfo, QVector<PNVPoint>& track, FLY_MODE flying_mode) {
	if(!track.isEmpty()) track.clear();
    was_manevr = false;

	unsigned pnv_number = getNumberPnv(pnvInfo, our, target, flying_mode);
	pnvInfo.pnv_number = pnv_number;
	PNV* pnv = &pnvInfo.pnvPath[pnv_number-1];

	// Начальная точка ПНВ
	double hs = ((*pnv)[0]->h1 != -1) ? (*pnv)[0]->h1 : pnvInfo.h_start;
	double vs = ((*pnv)[0]->v1 != -1) ? (*pnv)[0]->v1 : pnvInfo.v_start;
	PNVPoint startPoint(0.0, 0.0, hs, vs);
	startPoint.new_area = true;
	startPoint.type = PNVArea::Climb;
	track.push_back(startPoint);

	double cur_v = vs, cur_h = hs;
	unsigned area_count = pnv->size();
	for(size_t i = 0; i < area_count; ++i) {
		if((*pnv)[i] == nullptr) continue;		// балансовым участка выставлено nullptr
        integrateArea((*pnv)[i], pnvInfo, cur_v, cur_h, target.y, track, target);
		track.back().new_area = true;
		track.back().area = pnvInfo.getAreaNumber((*pnv)[i]);
		pnvInfo.maxH = std::max(pnvInfo.maxH, cur_h);
	}
    pnvInfo.time = pnvInfo.dt * track.size();

    was_manevr = true;

}
