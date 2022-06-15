#include "pointingmodel.h"

#include <QDebug>

using namespace Model;


/*  ОБЩИЕ ФУНКЦИИ */

double Model::angleBetweenPoints(double x0, double y0, double x1, double y1) {
    double x = x1 - x0, y = y1 - y0;
    if(x == 0.0) return ( (y > 0.0) ? GPI2 : 3*GPI2 );
    double theta(atan(y/x));
    if(x > 0.0) return ( (y >= 0.0) ? theta: 2*GPI + theta );
    return GPI + theta;
}
double Model::distance(double dx, double dy, double dz) {
    return sqrt(dx*dx + dy*dy + dz*dz);
}
template <typename T>
static T sign(const T& tmp) {
    return (tmp == T(0)) ? 0 :
        (tmp > T(0) ? 1 : -1);
}
double Model::convertTo2PI(double angle) {
    //int k = std::floor(angle / 2*GPI);
    //angle -= 2*k*GPI;

    //angle = (angle >= 0) ? angle : 2*GPI - angle;
    double p2 = 2*GPI;
    while(angle >= p2) angle -= p2;
    while(angle < 0) angle += p2;

    return angle;
}
double Model::fromCourceToxAxisAngle(double cource) {
    cource = 2*GPI - (cource - GPI2);
    return convertTo2PI(cource);
}
double Model::fromxAxisAngleToCource(double angle) {
    angle = 2*GPI - angle + GPI2;
    return convertTo2PI(angle);
}
double Model::section(double startAngle, double currentAngle, bool omega) {
    double sect = startAngle - currentAngle;

    sect = (!omega && sect < 0) ? 2*GPI - sect : sect;
    sect = (omega && sect > 0) ? 2*GPI - sect : sect;

    return std::abs(sect);

}
double Model::getManevrTime(double inAngle, double outAngle, double omega) {
    double sect = (section(inAngle, outAngle, omega > 0));
    return std::abs(sect / omega);
}

inline void pointInLocCoordSys(const coordinates& center, const coordinates& point, double& x21, double& y21, double& z21) {
    x21 = point.X - center.X,
    y21 = point.Y - center.Y,
    z21 = point.Z - center.Z;
}


/* ЗАПОЛНЕНИЕ БАЗОВЫХ ХААРКТЕРИСТИК, В МОДЕЛЕ БУДЕТ ПО-ДРУГОМУ */

AreaType Model::getPnvAreaType(unsigned type) {
    switch(type) {
        case 1: return AreaType::uparea;
        case 2: return AreaType::downarea;
        case 3: return AreaType::fastarea;
        case 4: return AreaType::slowarea;
    }
}

void ClimbProgramConsts::createPath(unsigned number_pnv, QVector<unsigned> areaNumbers) {
    if(pnvPath.size() <= number_pnv) pnvPath.resize(number_pnv);
    pnvPath[number_pnv-1].resize(areaNumbers.size());
    double cv = areas[areaNumbers[0]-1].v2;
    double ch = areas[areaNumbers[0]-1].h2;
    double time(0.0);

    for(size_t i = 0; i < areaNumbers.size(); ++i) {
        if(areaNumbers[i] == 0) {
            pnvPath[number_pnv-1][i] = nullptr;
            continue;
        }
        else pnvPath[number_pnv-1][i] = &areas[areaNumbers[i]-1];

        if(pnvPath[number_pnv-1][i]->type == AreaType::uparea || pnvPath[number_pnv-1][i]->type == AreaType::downarea)
            time += std::abs( (pnvPath[number_pnv-1][i]->h2 - ch) / pnvPath[number_pnv-1][i]->vy );
        else  time += std::abs( (pnvPath[number_pnv-1][i]->v2 - cv) / pnvPath[number_pnv-1][i]->vc );

        cv = pnvPath[number_pnv-1][i]->v2;
        ch= pnvPath[number_pnv-1][i]->h2;
    }

    pnvTime[number_pnv-1] = time;
}

void ClimbProgramConsts::setVN(double vn1, double vn2, double vn3, double vn4) {
    VN1 = vn1; VN2 = vn2; VN3 = vn3; VN4 = vn4;
}

unsigned ClimbProgramConsts::getAreaNumber(const ClimbSectionConsts* area) const {
    for(int i = 0; i < int(areas.size()); ++i) {
        if(area == &areas[i]) return (i+1);
    }

    return -1;
}

ClimbProgramConsts::ClimbProgramConsts() {
    pnvTime.resize(12);
}

ClimbSectionConsts::ClimbSectionConsts() {

}



/* ПРЕДСТАВЛЕНИЕ ТРАЕКТОРИИ В МОДЕЛЕ */


Area::Area(AreaType _type, coordinates sp, double _time, double _lenght, double _fuel)
    : type(_type), time(_time), lenght(_lenght), fuel(_fuel), start_point(sp) {

}
Area::Area(AreaType _type, motionParamLA pos) {
    type = _type;
    start_time = pos.t;
    start_point = pos;
    v0 = pos.v;
    kurs0 = pos.psi;
    theta0 = pos.theta;
}
void Area::setPosition(const motionParamLA& pos) {
    start_point = pos;
    start_time = pos.t;
    h0 = pos.Y;
    v0 = pos.v;
    kurs0 = pos.psi;
    theta0 = pos.theta;
}


PnvArea::PnvArea(AreaType _type, coordinates sp)
    : Area(_type, sp) {

}
PnvArea::PnvArea(AreaType _type, motionParamLA pos)
    : Area(_type, pos) {

}
void PnvArea::integrate(motionParamLA& pos) {
    setPosition(pos);

    if(type == AreaType::uparea || type == AreaType::downarea) lenght = v0*time;
    else lenght = v0*time + (vc*time*time) / 2.0;
    theta0 = std::acos(vy / v0);

   //end_point.X = start_point.X + lenght*sin(GPI2 - theta0)*sin(GPI2 - kurs0);
   //end_point.Z = start_point.Z + lenght*sin(GPI2 - theta0)*cos(GPI2 - kurs0);
   //end_point.Y = start_point.Y + lenght*cos(GPI2 - theta0);
    double v_hor = v0 - std::abs(vy);
    double _vx = v_hor*sin(GPI2 - kurs0), _vxc = vc*sin(GPI2 - kurs0);
    double _vz = v_hor*cos(GPI2 - kurs0), _vzc = vc*cos(GPI2 - kurs0);

    end_point.X = start_point.X + _vx*time + (_vxc*time*time) / 2.0;
    end_point.Z = start_point.Z + _vz*time + (_vzc*time*time) / 2.0;
    //end_point.X = start_point.X + time*(v0 - vy)*sin(GPI2 - kurs0);
    //end_point.Z = start_point.Z + time*(v0 - vy)*cos(GPI2 - kurs0);
    end_point.Y = start_point.Y + time*vy;

    double integrate_time = pos.t + time;
    pos = position(); pos.t = integrate_time;
}
void PnvArea::integrate_pos(motionParamLA& pos, double t) {
    t = ( (start_time - t) > time ) ? time : (start_time);
    double _vx = (v0 - vy)*sin(GPI2 - kurs0), _vxc = vc*sin(GPI2 - kurs0);
    double _vz = (v0 - vy)*cos(GPI2 - kurs0), _vzc = vc*cos(GPI2 - kurs0);

    double xt = start_point.X + _vx*t + (_vxc*t*t) / 2.0;
    double zt = start_point.Z + _vz*t + (_vzc*t*t) / 2.0;
    double yt = start_point.Y + t*vy;

    double vt = v0 + t*vc;

    pos = motionParamLA(xt, yt, zt, vt, kurs0, theta0, start_time + t);
}
void PnvArea::extrapolate(Track& track, double t, double dt) {
    double cv = v0;

    double cx = start_point.X, cy = start_point.Y, cz = start_point.Z;

    for(double t = 0.0; t < time; t += dt) {
       //cx += cv*sin(GPI2 - theta0)*sin(GPI2 - kurs0);
       //cy += cv*cos(GPI2 - theta0);
       //cz += cv*sin(GPI2 - theta0)*cos(GPI2 - kurs0);

        cx += dt*(cv - std::abs(vy))*sin(GPI2 - kurs0);
        cy += dt*vy;
        cz += dt*(cv - std::abs(vy))*cos(GPI2 - kurs0);

        motionParamLA pos(cx, cy, cz, cv, kurs0, theta0, track.size());
        pos.psi = kurs0; pos.theta = theta0;

        track.push_back(pos);

        cv += vc;
    }
}
motionParamLA PnvArea::position() {
    return motionParamLA(end_point.X, end_point.Y, end_point.Z, v2, kurs0, theta0, time);
}


HorizontalLine::HorizontalLine(coordinates sp)
    : Area(AreaType::line, sp) {

}
HorizontalLine::HorizontalLine() : Area(AreaType::line, motionParamLA()) {
    kurs0 = -1.0;
}
void HorizontalLine::integrate(motionParamLA& pos) {
    if(kurs0 < 0)  setPosition(pos);
    else {
        double tmp_kurs = kurs0;
        setPosition(pos);
        kurs0 = tmp_kurs;
    }

    time = lenght / v0;

    double fix = fromCourceToxAxisAngle(kurs0);
    end_point = coordinates(start_point.X + lenght*sin(fix),
                            start_point.Y,
                            start_point.Z + lenght*cos(fix));

    double integrate_time = pos.t + time;
    pos = position(); pos.t = integrate_time;

}
void HorizontalLine::integrate_pos(motionParamLA& pos, double t) {
    t = (t < 0) ? time : std::min(time, t);
    double l = v0*t;
    double fix = fromCourceToxAxisAngle(kurs0);

    double xt = start_point.X + l*sin(fix);
    double yt = start_point.Y;
    double zt = start_point.Z + l*cos(fix);

    pos = motionParamLA(xt, yt, zt, v0, kurs0, theta0, start_time + t);
}
void HorizontalLine::extrapolate(Track& track, double t, double dt) {
     time = (t < 0) ? time : std::min(time, t);

     double vz = v0*std::cos(GPI2 - kurs0),
            vx = v0*std::sin(GPI2 - kurs0);
     double cz = start_point.Z + vz, cx = start_point.X + vx;

     for(double t = 0; t < time; t += dt) {
         motionParamLA pos(cx, start_point.Y, cz, v0, kurs0, theta0, track.size());

         track.push_back(pos);
         cz += vz; cx += vx;
     }
}
motionParamLA HorizontalLine::position() {
    return motionParamLA(end_point.X, end_point.Y, end_point.Z, v0, kurs0, theta0, time);
}


TurnManevr::TurnManevr(coordinates sp)
    : Area(AreaType::manevr, sp) {

}
TurnManevr::TurnManevr(motionParamLA position)
    : Area(AreaType::manevr, position) {

}
TurnManevr::TurnManevr() : Area(AreaType::manevr, motionParamLA()) {

}
double TurnManevr::getKursByAngle() {
    double kurs = (omega > 0) ? 2*GPI - angle_out : 2*GPI - angle_out -GPI ;
    if(kurs < 0) kurs = 2*GPI + kurs;
    return kurs;
}
double TurnManevr::count_radius(double v, double gamma) {
    return std::abs( (v*v) / (G*tan(gamma)) ) ;
}

void TurnManevr::clear() {
    center = {0, 0, 0};
    time = 0;

}

void TurnManevr::integrate(motionParamLA& pos) {
    setPosition(pos);
    /* Центр первого разворота */
    double fi1x = GPI2 - kurs0;				// Угол относительно оси абсцисс
    double k = (-1) / (tan(fi1x));				// Коэффициент прямой перпендикулярной вектору скорости, (на этой прямой центр окружности)
    double kg = atan(k);						// Угол относительно оси абсцисс прямой y = kx
    double cz1 = radius*cos(kg), cx1 = radius*sin(kg);	// Координаты центра окружности радиуса
    // Если курс от 90 до 270 то получили координаты центра для разворота по часовой, меняем их на против часовой
    if( !(kurs0 > GPI2 && kurs0 < 3*GPI2)) { cz1 = -cz1; cx1 = -cx1; }
    if(gamma > 0) { cz1 = -cz1; cx1 = -cx1; }		// Если нужно по часовой, то меняем
    cz1 += start_point.Z; cx1 += start_point.X;					// Перенесли центр к текущему расположении точки на окружности

    center = coordinates(cx1, start_point.Y, cz1);

    // Углы входа и выхода в разворот
    angle_in = angleBetweenPoints(cz1, cx1, start_point.Z, start_point.X);
    angle_out = (gamma < 0) ? angle_in + alpha : angle_in - alpha;
    convertTo2PI(angle_out);				// Если вышли за 0...2PI
    // Угловая скорость и время
    double gm1 = atan((v0*v0) / (G*radius));
    omega = (double)sign(gamma)*(-1) * ( v0 / radius );
    //if(omega > 0) omega = -omega;
    time = getManevrTime(angle_in, angle_out, omega);	// Сколько потребуетс времени на разворот на alpha1
    lenght = radius*std::abs(alpha);

    end_point = coordinates(center.X + radius*sin(angle_out), start_point.Y, center.Z + radius*cos(angle_out));

    double integrate_time = pos.t + time;
    pos = position(); pos.t = integrate_time;
}
void TurnManevr::integrate_pos(motionParamLA& pos, double t) {
    if(radius <= 0.0) return;
    //t = ( (start_time - t) > time ) ? time : (start_time);
    double sec = ( t / time ) * alpha;
    //double out_angle = (omega < 0) ? angle_in + sec : angle_in - sec;
    //convertTo2PI(out_angle);
    double out_angle = angle_in + t*omega;

    double xt = center.X + radius*sin(out_angle);
    double yt = start_point.Y;
    double zt = center.Z + radius*cos(out_angle);
    double kurst = (omega < 0) ? (kurs0 - sec) : (kurs0 + sec);
    convertTo2PI(kurst);

    pos = motionParamLA(xt, yt, zt, v0, kurst, theta0, start_time + t);
}
void TurnManevr::extrapolate(Track& track, double t, double dt) {
    time = (t < 0) ? time : std::min(time, t);

    double z0 = center.Z, x0 = center.X;
    double currentAngle = angle_in + omega;
    double kt = kurs0;
    for(t; t < time; t += dt) {
        kt += omega;
        double zt(z0 + radius*cos(currentAngle)), xt(x0 + radius*sin(currentAngle));
        track.push_back(motionParamLA(xt, start_point.Y, zt, v0, kt, theta0, track.size()));
        currentAngle += omega;
    }
}
motionParamLA TurnManevr::position() {
    return motionParamLA(end_point.X, end_point.Y, end_point.Z, v0, getKursByAngle(), theta0, time);
}


/* МОДЕЛЬ */

PointingMathModel::PointingMathModel(Config&& _config)
    : config(_config){
}
bool PointingMathModel::pointingDefeatAircraftOnBase(const motionParamLA& our, const motionParamLA& target, double& timeDefeat) {
    set_model_info(our, target);

    // Определение и заполнение ПНВ
    unsigned pnv_number, pnv_group;
    pick_pnv(target, pnv_number, pnv_group);
    prepare_pnv_sections(our, target, pnv_number, pnv_group);
    count_pnv_characters();
    PnvPath* pnv = &pnvInfo.pnvPath[pnv_number];
    //pnv_time = pnvInfo.pnvTime[pnv_number];

    size_t index_pnv_area = 0;
    bool was_balance = false, was_turn = false;
    // Первичный набор высоты
    motionParamLA pos = our, cur_tar = target;
    double current_pointing_time(0.0), current_pointing_length(0.0);
    double current_pnv_time(0.0), current_pnv_length(0.0);

    auto make_turn = [&]() {
        integrateTarget(target, current_pointing_time, cur_tar);
        TurnManevr* turn_area = new TurnManevr(pos);
        turn_to_lead_point_modern(pos, cur_tar,
                                  current_pnv_time, current_pnv_length,
                                  index_pnv_area, turn_area);
        areas.push_back(turn_area);
        pos = turn_area->position();

        current_pointing_time += turn_area->time;
        current_pointing_length += turn_area->lenght;
    };

    while(index_pnv_area < pnv_areas.size()) {
        if((*pnv)[index_pnv_area] == nullptr && !was_balance) {
           integrateTarget(target, current_pointing_time, cur_tar);
           TurnManevr* turn = new TurnManevr(pos);
           HorizontalLine* line = new HorizontalLine();
           bool possible = optimal_turn(pos, cur_tar, turn, line);
           turn->integrate(pos);
           areas.push_back(turn);
           current_pointing_time += turn->time;
           current_pointing_length += turn->lenght;


            PnvArea* balance_area  = build_balance_section(pos, target, current_pointing_time);
            balance_area->integrate(pos);
            areas.push_back(balance_area);

            current_pointing_time += balance_area->time;
            current_pointing_length += balance_area->lenght;
            pnv_time += balance_area->time;
            pnv_length += balance_area->lenght;
            was_balance = true;
        }
        if(pnv_areas[index_pnv_area]->type == AreaType::fastarea && !was_turn) {
            make_turn();
            was_turn = true;
        }

        PnvArea* pnv_area = pnv_areas[index_pnv_area];
        pnv_area->integrate(pos);
        areas.push_back(pnv_area);

        current_pointing_time += pnv_area->time;
        current_pointing_length += pnv_area->lenght;
        current_pnv_time += pnv_area->time;
        current_pnv_length += pnv_area->lenght;
        index_pnv_area++;
    }

    qDebug() << "the end PnvPath";

    // Наведение в горизонтальной плоскости
    motionParamLA target_current_pos;
    integrateTarget(target, current_pointing_time, target_current_pos);

    bool result = pointingDefeatAircraftAzimuth(pos, target_current_pos);
    if(!result) qDebug() << "Наведение отменяется";

    return result;
}


bool PointingMathModel::pointingDefeatAircraftAzimuth(const motionParamLA& our, const motionParamLA& target) {
    //clear_model();
    //set_model_info(our, target);
    //motionParamLA subour = our;
    //if(target.v >= our.v) subour.v = target.v + 10;

    motionParamLA cur_pos = our;

    bool is_pointing;
    switch(config.method) {
        case Method::MANEVR: {
            TurnManevr* turn1 = new TurnManevr(cur_pos), *turn2 = new TurnManevr();
            HorizontalLine* line = new HorizontalLine();
            is_pointing = (config.find_optimal_manevr) ? optimal_maneuver(our, target, turn1, line, turn2)
                                                       : two_turn_maneuver(our, target, turn1, line, turn2);
            if(!is_pointing) break;

            turn1->integrate(cur_pos);
            line->integrate(cur_pos);
            turn2->integrate(cur_pos);

            areas.push_back(turn1);
            areas.push_back(line);
            areas.push_back(turn2);
            break;
        }

        case Method::INTERCEPTION: {
            HorizontalLine* line = new HorizontalLine(cur_pos);
            is_pointing = interception_dl(our, target, line);
            if(!is_pointing) break;

            line->integrate(cur_pos);
            areas.push_back(line);
            break;
        }

        case Method::TURN_INTERCEPTION: {
            TurnManevr* turn = new TurnManevr(cur_pos);
            HorizontalLine* line = new HorizontalLine();
            is_pointing = (config.find_optimal_manevr) ? optimal_turn(our, target, turn, line)
                                                       : turn_interception_dl(our, target, turn, line);
            if(!is_pointing) break;

            turn->integrate(cur_pos);
            line->integrate(cur_pos);

            areas.push_back(turn);
            areas.push_back(line);
            break;
        }
    }

    return is_pointing;
}

void PointingMathModel::extrapolate(Track& track) {
    track.clear();
    track.reserve(time / config.dt);

    for(size_t i = 0; i < size_t(areas.size()); ++i)
        areas[i]->extrapolate(track);
}
motionParamLA PointingMathModel::get_position(double t) {
    t -= areas.front()->start_time;
    size_t i(0);
    for(i; i < size_t(areas.size() - 1); ++i) {
        if(t - areas[i]->time < 0.0) break;
        t -= areas[i]->time;
    }

    motionParamLA pos;
    areas[i]->integrate_pos(pos, t);

    return pos;
}

motionParamLA PointingMathModel::get_first_position() {
    Area* area = areas[0];
    coordinates coord = area->start_point;
    return motionParamLA(coord.X, coord.Y, coord.Z, area->v0, area->kurs0, area->theta0, 0.0);
}


/* Методы наведения */

bool PointingMathModel::two_turn_maneuver(const motionParamLA& our, const motionParamLA& target, TurnManevr* turn1, HorizontalLine* line, TurnManevr* turn2) {
    double B, P, v1, v2, g0;
    int change_m = 1;
    maneuver_conversion_in(our, target, B, P, v1, v2, g0, change_m);

    /* Исходные данные */
    long double p = config.q_zad;
    long double sk = config.dl/1000.0, vk = v1;
    int m1 = sign(config.gamma1), m2 = sign(config.gamma2);
    long double Rp = std::abs( (our.v*our.v) / (1000*G*tan(config.gamma1)) ), Rmin = std::abs( (our.v*our.v) / (1000*G*tan(30*d2r)) );
    long double r1 = Rp, r2 = Rp;

    m1 *= change_m; m2 *= change_m;

    /* Дополнительные переменные */
    long double a1 = v2 / v1,
            a2 = (2*v2) / (v1 + vk);

    /* Для начала итераций */
    long double cg = maneuver_init_solution(B, P, p, sk, a1);
    long double deltaR = 2.5;

    /* Начало рассчетов */
    r1 = r2 = deltaR;

    /* Постоянные дифференциалы */
    const long double de_dr	= a1*(m2 - m1) - m1*cos(g0) + m2*cos(p);
    const long double dd1_dr	= -m1*a1 + m2*a2;
    const long double ds_dr = a1*(m1*cos(g0) - m2*cos(p)) - m2 + m1;

    /* Результаты расчетов */
    long double g1,					// курс на прямолинейном учатске
                R_g1 = 0,			// Радиус разворотов
                C = 0,				// Длина прямолинейного учатска
                L,					// Путь пройденный цельлю до точки перехвата
                alpha1,				// Величина первого разворота (в указанную сторону)
                alpha2;				// Величина второго разворота (в указанную сторону)

    size_t iter_count(0);
    while(R_g1 < Rp && C >= 0 && iter_count++ < 50) {

        /* Вроде это в итерации */
        long double i = (cg - g0 >= 0) ? 0 : 1;
        long double j = (p-cg >= 0) ? 0: 1;

        /* Значения */
        long double e	= (m2*r2 - m1*r1)*a1 - B + sk*sin(p) - m1*r1*cos(g0) + m2*r2*cos(p);
        long double d1	= a2*m2*r2 - a1*m1*r1;
        long double d2	= -P -sk*cos(p) + m2*r2*sin(p) - m1*r1*sin(g0) +
                        a1*(GPI + (GPI + g0 - 2*GPI*i)*m1)*r1 +
                        a2*(GPI + (GPI - p - 2*GPI*j)*m2)*r2;
        long double s	= (B - sk*sin(p) + m1*r1*cos(g0) - m2*r2*cos(p))*a1 - m2*r2 + m1*r1;

        /* Дифференциалы */
        long double dd2_dr = m2*sin(p) - m1*sin(g0) + a1*(GPI + (GPI + g0 -2.0*GPI*i)*m1) + a2*(GPI + (GPI - p - 2.0*GPI*j)*m2);

        long double dcg_dr = (de_dr*cos(cg) + (dd1_dr*cg + dd2_dr)*sin(cg) + ds_dr) / ((e-d1)*sin(cg) - (d1*cg + d2)*cos(cg));

        /* Приращение */
        cg += dcg_dr*deltaR;

        /* Радиус */
        long double R_cg = ( (P+sk*cos(p))*sin(cg) + (B - sk*sin(p))*(cos(cg) -a1 ) ) / (de_dr*cos(cg) + (dd1_dr*cg + dd2_dr)*sin(cg) + ds_dr);

        if(R_cg >= Rp || C < 0) break;

        // Записываем новые значения
        g1 = cg;
        R_g1 = R_cg;
        C		= (B - sk*sin(p) + m1*r1*cos(g0) - m2*r2*cos(p) + (m2*r2 - m1*r1)*cos(cg)) / sin(cg);

        /* Другие неизвестные */
        long double delta_cg1 = cg - g0 + 2.0*GPI*i;
        alpha1 = GPI + (GPI - delta_cg1)*m1;

        long double delta_cg2 = p - cg + 2.0*GPI*j;
        alpha2 = GPI + (GPI - delta_cg2)*m2;

        L       = a1*alpha1*r1 + a2*alpha2*r2 + a1*C;

        // Приращение по радиусам разворотов для следующей итерации
        r1 += deltaR;
        r2 += deltaR;
    }

    if(R_g1 <= 0.0 || C <= 0.0)  {
        turn1->possible = line->possible = turn2->possible = false;
        return false;
    }

    // В обычный вид
    maneuver_conversion_out(g1, R_g1, C, target.psi);
    m1 *= change_m; m2 *= change_m;

    // Запись результата вычисления
    turn1->setPosition(our);
    turn1->radius = R_g1;
    turn1->alpha = alpha1;
    turn1->gamma = sign(m1);

    line->kurs0 = -1.0;
    line->lenght = C;
    line->v0 = our.v;

    turn2->radius = R_g1;
    turn2->alpha = alpha2;
    turn2->gamma = sign(m2);

    return true;
}
bool PointingMathModel::interception(const motionParamLA& our, const motionParamLA& target, HorizontalLine* area) {
    double z21 = target.Z - our.Z, x21 = target.X - our.X; // В местную систему координат (центр ЛА)
    double psi = angleBetweenPoints(0, 0, z21, x21);
    double fi2 = GPI2 + target.psi + psi;
    double v1 = our.v, v2 = target.v;

    double dist = distance(z21, x21, 0);				// Начальная дистанция до цели
    double fi	= asin((v2/v1) * sin(fi2))		;		// Относительно оси абсцисс
    double t = dist / (v1*cos(fi) + v2*cos(fi2));		// Необходимое время полета до цели с курсом fi
    fi += psi;
    fi = GPI2 - fi;										// Относительно оси ординат (курс)

    area->possible = our.v > target.v;
    area->kurs0 = fi;
    area->time = t;
    area->lenght = t*our.v;

    return area->possible;
}
bool PointingMathModel::interception_dl(const motionParamLA& our, const motionParamLA& target, HorizontalLine* area) {
    double z21 = target.Z - our.Z, x21 = target.X - our.X; // В местную систему координат (центр ЛА)
    double psi = angleBetweenPoints(0, 0, z21, x21);
    double fi2 = GPI2 + target.psi + psi;
    double v1 = our.v, v2 = target.v;
    double dist = distance(z21, x21, 0);				// Начальная дистанция до цели
    double dl = config.dl;

    double m = v1 / v2;
    double t = pow((m*dl + dist*cos(fi2)), 2) + (dist*dist -dl*dl)*(m*m - 1.0);
    t = sqrt(t) - (m*dl + dist*cos(fi2));
    t = t / (v2*(m*m - 1.0));

    double fi = asin( ( v2*t/(v1*t + dl) ) * sin(fi2) );
    fi += psi;
    fi = GPI2 - fi;

    if(fi < 0) fi = 2*GPI + fi;


    area->possible =  (t == t && t > 0);	// t == t не работает только в случае NaN
    area->kurs0 = fi;
    area->time = t;
    area->lenght = t*our.v;

    return area->possible;
}
bool PointingMathModel::turn_interception_dl(const motionParamLA& our, const motionParamLA& target,TurnManevr* turn, HorizontalLine* line) {
    // Привели к более удобной СК для расчета разворота
    motionParamLA subour, subtarget, pos(our);
    make_turn_subtask(our, target, subour, subtarget);
    double x21 = subtarget.X, z21 = subtarget.Z, f21 = subtarget.psi;

    /* Решение задачи методом касательных */
    double radius = TurnManevr::count_radius(subour.v, config.gamma1);
    double gamma = config.gamma1;
    turn->radius = radius;
    turn->integrate(subour);
    double z0 = turn->center.Z, x0 = turn->center.X;

    // Оптимизируемая функция угла разворота
    auto f = [&](double alpha) {
        double a1 = target.v / our.v;
        motionParamLA our2;
        if(gamma < 0) our2 = motionParamLA(x0 + radius*sin(alpha), 0.0, z0 + radius*cos(alpha), subour.v, 0.0, 0.0, 0.0);
        else our2 = motionParamLA(x0 + radius*sin(GPI - alpha), 0.0, z0 + radius*cos(GPI - alpha), subour.v, 0.0, 0.0, 0.0);
        motionParamLA tar2(x21 + a1*alpha*radius*sin(GPI2 - f21), 0.0, z21 + a1*radius*alpha*cos(GPI2 - f21), subtarget.v, f21, 0.0, 0.0);

        HorizontalLine* line = new HorizontalLine();
        interception_dl(our2, tar2, line);
        line->integrate(our2);
        double cource = line->kurs0;
        double manevr_cource = (gamma > 0) ? alpha : (2.0*GPI - alpha);
        return manevr_cource - cource;
    };

    // Численный метод
    double alpha = 0.0, prev = 0.0, a = 0.0, b = 2.0*GPI;
    double eps = 0.01;

    size_t iter_counts = 0;
    do {
         prev = alpha;
         double f_prev = f(prev), f_a = f(a), f_b = f(b);
         alpha = prev - (f_prev / (f_b - f_prev)) * (b - prev);
    } while(std::abs(prev - alpha) > eps && iter_counts++ < 50);

    if(iter_counts > 50) return false;


    turn->setPosition(our);
    turn->alpha = alpha; turn->gamma = config.gamma1;
    turn->integrate(pos);

    motionParamLA target_pos;
    integrateTarget(target, turn->time, target_pos);
    interception_dl(pos, target_pos, line);

    return true;
}

bool PointingMathModel::pursuit() {
    return false;
}

/* Формирование подзадач */
void PointingMathModel::make_turn_subtask(const motionParamLA& our, const motionParamLA& target, motionParamLA& subour, motionParamLA& subtar) {
    double x21, y21, z21;
    pointInLocCoordSys(our, target, x21, y21, z21);

    double turn_angle = 2.0*GPI - our.psi;

    subour = our;
    subour.psi = 0.0;
    subour.X = subour.Z = 0;

    subtar = target;
    subtar.psi = target.psi - our.psi;
    if(subtar.psi < 0) subtar.psi += 2.0*GPI;
    subtar.Z = z21 * cos(turn_angle) + x21 * sin(turn_angle);
    subtar.X = x21 * cos(turn_angle) - z21 * sin(turn_angle);
}

/* Методы корректировки */
void PointingMathModel::turn_to_lead_point_modern(const motionParamLA& our, const motionParamLA& target, double time, double length, size_t index_section, TurnManevr*& area) {
    // Учтем отставание
    double time_left = pnv_time - time,
           length_left = pnv_length - length;
    double length_vert_left(0.0);
    for(size_t i = index_section, size = pnv_areas.size(); i < size; ++i)
        length_vert_left += std::abs(pnv_areas[i]->vy) * pnv_areas[i]->time;

    double v_midl = length_left / time_left;
    double v_hor_midl = v_midl - (length_vert_left / time_left);
    double v_left = std::abs(program_v - v_hor_midl);

    double dist = config.dl;                                // Сохраним значение
    config.dl = -(time_left * v_left + config.dl);      // Фиктивное опережение

    motionParamLA subour(our);
    subour.v = program_v;
    TurnManevr* subarea = new TurnManevr(subour);


    HorizontalLine* line = new HorizontalLine();
    //turn_interception_dl(subour, target, subarea, line);
    bool possible = optimal_turn(subour, target, subarea, line);

    area->setPosition(our);
    area->gamma = subarea->gamma;
    area->alpha = subarea->alpha;
    area->radius = TurnManevr::count_radius(our.v, config.gamma1);
    motionParamLA pos(our);
    area->integrate(pos);

    if(!possible) {
        motionParamLA subour(our);
        subour.v = target.v + 10;
        optimal_turn(subour, target, area, line);
    }

    //motionParamLA pos(our);
    //area->integrate(pos);
    config.dl = dist;                                       // Вернули исходое значение
}

void PointingMathModel::turn_to_lead_point_modern2(const motionParamLA& our, const motionParamLA& target, double time, double length, size_t index_ection, TurnManevr*& area) {

    HorizontalLine* line = new HorizontalLine();
    bool possible = optimal_turn(our, target, area, line);

    motionParamLA pos(our);
    area->integrate(pos);

    if(!possible) {
        motionParamLA subour(our);
        subour.v = target.v + 10;
        optimal_turn(subour, target, area, line);
    }


}

void PointingMathModel::turn_to_lead_point(const motionParamLA& our, const motionParamLA& target, double time, TurnManevr* area) {
    double dist = config.dl;
    config.dl = - (time * our.v + config.dl);     // Фиктивное опережение

    HorizontalLine* line = new HorizontalLine();
    interception_dl(our, target, line);

    motionParamLA pos = our;
    area->radius = our.v*our.v / (G*tan(30*d2r));
    area->omega = 1;
    area->alpha = section(our.psi, line->kurs0, true);  // вычисление минимального разворта true или false
    area->integrate(pos);

    config.dl = dist;      // Вернули исходое значение
}


/* Вспомогательные методы */
void PointingMathModel::maneuver_conversion_in(const motionParamLA& our, const motionParamLA& target, double& B, double& P, double& v1, double& v2, double& g0, int& change_m) {
    double gtarget = fromCourceToxAxisAngle(target.psi);	// Угол относительно оси абсцисс
    double gour	   = fromCourceToxAxisAngle(our.psi);

    P = (our.Z - target.Z)*cos(gtarget) + (our.X - target.X)*sin(gtarget);
    B = (our.Z - target.Z)*sin(gtarget) - (our.X - target.X)*cos(gtarget);
    B /= 1000; P /= 1000;

    g0 = (gour - gtarget);

    if(g0 < 0) g0 += 2*GPI;
    if(B < 0) {
        change_m = -change_m;
        B = -B;
        g0 = 2*GPI - g0;
    }

    v1 = our.v / 1000.0; v2 = target.v / 1000.0;
}
void PointingMathModel::maneuver_conversion_out(long double& g1, long double& R, long double& C, double psi2) {
    R *= 1000; // в метры
    C *= 1000;
    C = std::abs(C);
    double gtarget = fromCourceToxAxisAngle(psi2);	// Угол относительно оси абсцисс
    g1 = g1 + gtarget;
    g1 = fromxAxisAngleToCource(g1);
}
double PointingMathModel::maneuver_init_solution(double b, double p, double fip, double dl, double a1) {
    double e  = -b + dl*sin(fip);
    double d2 = -p - dl*cos(fip);
    double s  = a1*(b - dl*sin(fip));

    double a = e*e + d2*d2;
    double b2 = 2.0*s*e;
    double c = s*s - d2*d2;

    double x;
    if(p >= 0) x = (-b2 - sqrt(b2*b2 - 4*a*c)) / (2.0*a);
    else x = x = (-b2 + sqrt(b2*b2 - 4*a*c)) / (2.0*a);

    double gamma0 = acos(x);

    return gamma0;
}
bool PointingMathModel::optimal_maneuver(const motionParamLA& our, const motionParamLA& target, TurnManevr*& turn1, HorizontalLine*& line, TurnManevr*& turn2) {
    double gm1 = config.gamma1, gm2 = config.gamma2;
    double gamma1 = std::abs(config.gamma1), gamma2 = std::abs(config.gamma2);
    vector<TurnManevr> turns(8);
    vector<HorizontalLine> lines(4);
    vector<bool> results(4);

    config.gamma1 = gamma1, config.gamma2 = gamma2;
    results[0] = two_turn_maneuver(our, target, &turns[0], &lines[0], &turns[1]);

    config.gamma1 = -gamma1, config.gamma2 = gamma2;
    results[1] = two_turn_maneuver(our, target, &turns[2], &lines[1], &turns[3]);

    config.gamma1 = gamma1, config.gamma2 = -gamma2;
    results[2] = two_turn_maneuver(our, target, &turns[4], &lines[2], &turns[5]);

    config.gamma1 = -gamma1, config.gamma2 = -gamma2;
    results[3] = two_turn_maneuver(our, target, &turns[6], &lines[3], &turns[7]);

    int bestvar = -1;
    double min_lenght = std::numeric_limits<double>::max();
    for(size_t i = 0; i < 4; ++i) {
        if(results[i]) {
            double lenght = turns[i*2].radius*turns[i*2].alpha + lines[i].lenght + turns[i*2 + 1].radius*turns[i*2 + 1].alpha;
            bestvar = (min_lenght > lenght) ? i : bestvar;
            min_lenght = std::min(min_lenght, lenght);
        }
    }

    if(bestvar != -1) {
        turn1 =  new TurnManevr(turns[bestvar*2]);
        line = new HorizontalLine(lines[bestvar]);
        turn2 = new TurnManevr(turns[bestvar*2 + 1]);
    }

    config.gamma1 = gm1, config.gamma2 = gm2;
    return (bestvar != -1);
}
bool PointingMathModel::optimal_turn(const motionParamLA& our, const motionParamLA& target,TurnManevr*& turn, HorizontalLine*& line) {
    double gm1 = config.gamma1;
    double gamma1 = std::abs(config.gamma1);
    TurnManevr turn1, turn2;
    HorizontalLine line1, line2;

    config.gamma1 = gamma1;
    bool res1 = turn_interception_dl(our, target, &turn1, &line1);

    config.gamma1 = -gamma1;
    bool res2 = turn_interception_dl(our, target, &turn2, &line2);

    motionParamLA pos(our);
    if(res1 && res2) {
        turn = (turn1.lenght < turn2.lenght) ? new TurnManevr(turn1) : new TurnManevr(turn2);
        line = (turn1.lenght < turn2.lenght) ? new HorizontalLine(line1) : new HorizontalLine(line2);

        turn->integrate(pos);
    }
    else if(res1 || res2) {
        turn = (res1) ? new TurnManevr(turn1) : new TurnManevr(turn2);
        line = (res1) ? new HorizontalLine(line1) : new HorizontalLine(line2);

        turn->integrate(pos);
    }

    config.gamma1 = gm1;
    return (res1 || res2);
}

PnvArea* PointingMathModel::build_balance_section(const motionParamLA& cur_our, const motionParamLA& tar, double cur_time) {
    PnvArea* balance_area = new PnvArea(balance, cur_our);
    motionParamLA cur_tar;
    integrateTarget(tar, cur_time, cur_tar);

    HorizontalLine line(cur_our);
    interception_dl(cur_our, cur_tar, &line);

    double cur_dist = (line.possible) ? line.lenght : cur_our.distance(cur_tar);
    double balance_dist = std::abs(cur_dist - pnv_length) / 1.25;
    double balance_time = balance_dist / cur_our.v;

    balance_area->time = balance_time;
    balance_area->lenght = balance_dist;
    balance_area->vc = balance_area->vy = 0.0;
    balance_area->h2 = balance_area->h0;
    balance_area->v2 = balance_area->v0;

    return balance_area;
}


/* Подготовительные метод */
void PointingMathModel::pick_pnv(const motionParamLA& target, unsigned& pnv_number, unsigned& pnv_group) {
    if(target.v < pnvInfo.VN1) pnv_group = 1;
    else if(target.v < pnvInfo.VN2) pnv_group = 2;
    else if(target.v < pnvInfo.VN3) pnv_group = 3;
    else pnv_group = 4;

    pnv_number = (pnv_group-1)*3 + (int)(1);		// 0...9 + 1...3 = 1...12
    program_v = pnvInfo.pnvPath[pnv_number].back()->v2;
}
void PointingMathModel::prepare_pnv_sections(const motionParamLA& our, const motionParamLA& target, unsigned pnv_number, unsigned pnv_group) {
     motionParamLA cur_pos(our);
     const PnvPath& pnv = pnvInfo.pnvPath[pnv_number];

    for(size_t i = 0; i < size_t(pnv.size()); ++i) {
        ClimbSectionConsts* sectionConsts = pnv[i];
        if(!sectionConsts) continue;
        PnvArea* area = build_pnv_section(cur_pos, target, *sectionConsts);
        area->integrate(cur_pos);
        pnv_areas.push_back(area);

        pnv_time += area->time;
        pnv_length += area->lenght;
    }

}
PnvArea* PointingMathModel::build_pnv_section(const motionParamLA& current_position, const motionParamLA& target, const ClimbSectionConsts& sectionConst) {
    AreaType type = sectionConst.type;
    PnvArea* area = new PnvArea(type, current_position);

    area->h0 = current_position.Y;
    area->h2 = (sectionConst.h2 > 0) ? sectionConst.h2 : current_position.Y;
    area->v0 = current_position.v;
    area->v2 = (sectionConst.v2 > 0)  ? sectionConst.v2 : current_position.v;
    area->vc = sectionConst.vc;
    area->vy = sectionConst.vy;

    if(area->type == AreaType::fastarea || area->type == AreaType::slowarea)
        area->time = std::abs( (area->v2 - area->v0)/area->vc );
    else area->time = std::abs( (area->h2 - area->h0)/area->vy );

    if(area->type == AreaType::downarea /*&& area->h2 < target.Y*/) {
        area->h2 = target.Y;
        area->time = std::abs( (area->h2 - area->h0)/area->vy );
    }

    return area;
}
void PointingMathModel::buildArea(const motionParamLA& pos, PnvArea* area, const ClimbSectionConsts& character, const motionParamLA& target) {
    area->h0 = pos.Y;
    area->h2 = (character.h2 > 0) ? character.h2 : pos.Y;
    area->v0 = pos.v;
    area->v2 = (character.v2 > 0)  ? character.v2 : pos.v;
    area->vc = character.vc;
    area->vy = character.vy;

    if(area->type == AreaType::fastarea || area->type == AreaType::slowarea)
        area->time = std::abs( (area->v2 - area->v0)/area->vc );
    else area->time = std::abs( (area->h2 - area->h0)/area->vy );

    if(area->type == AreaType::downarea /*&& area->h2 < target.Y*/) {
        area->h2 = target.Y;
        area->time = std::abs( (area->h2 - area->h0)/area->vy );
    }
}
void PointingMathModel::count_pnv_characters() {
    pnv_time = pnv_length = 0.0;
    for(const PnvArea* area: pnv_areas) {
        pnv_time += area->time;
        pnv_length += area->lenght;
    }
    pnv_average_v = pnv_length / pnv_time;
}
unsigned PointingMathModel::getNumberPnv(const motionParamLA& our, const motionParamLA& target) {
    unsigned pnv_group;
    if(target.v < pnvInfo.VN1) pnv_group = 1;
    else if(target.v < pnvInfo.VN2) pnv_group = 2;
    else if(target.v < pnvInfo.VN3) pnv_group = 3;
    else pnv_group = 4;

    return (pnv_group-1)*3 + (int)(1);		// 0...9 + 1...3 = 1...12
}

/* Траектория цели */
void PointingMathModel::integrateTarget(const motionParamLA& target, double time, motionParamLA& pos2) {
    pos2 = target;
    pos2.Z = target.Z + time*target.v*cos(GPI2 - target.psi);
    pos2.X = target.X + time*target.v*sin(GPI2 - target.psi);
}
void PointingMathModel::extrapolateTarget(const motionParamLA& target, double time, Track& track) {
    double delta_t(1.0);
    double vz = target.v*cos(GPI2 - target.psi),
           vx = target.v*sin(GPI2 - target.psi);

    double zt = target.Z, xt = target.X;
    track.push_back(motionParamLA(xt, target.Y, zt, target.v, target.psi, target.theta, track.size()));

    for(double t(delta_t); t < time; t += delta_t) {
        zt += vz, xt += vx;
        track.push_back(motionParamLA(xt, target.Y, zt, target.v, target.psi, target.theta, track.size()));
    }

}

void PointingMathModel::clear_model() {
    for(size_t i = 0; i < size_t(areas.size()); ++i)
        delete areas[i];
    areas.clear();
}


double PointingMathModel::get_start_time() {
    return areas[0]->start_time;
}
double PointingMathModel::get_pointing_time() {
    double time(0.0);
    for(auto area: areas) time += area->time;

    return time;
}
void PointingMathModel::set_model_info(const motionParamLA& our, const motionParamLA& target) {
    our_start = our, target_start = target;
}
bool PointingMathModel::need_repoint(const motionParamLA& our, const motionParamLA& target) {
    return (target_start.psi != target.psi || target_start.v != target.v);
}


double PointingMathModel::test(const motionParamLA& our, const motionParamLA& target) {
    TurnManevr* turn = new TurnManevr(our);
    HorizontalLine* line = new HorizontalLine();
    motionParamLA pos(our);
    turn_interception_dl(our, target, turn, line);
    //optimal_turn(our, target, turn, line);

    turn->integrate(pos);
    line->integrate(pos);

    areas.push_back(turn);
    areas.push_back(line);
}

