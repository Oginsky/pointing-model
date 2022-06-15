/* ???? - ????? ?????? ????????? ?????????, ???????? ????????? ? ????????? ???
* ? ???? ????? ???????? ??????????? ??????? ?????? ??????, ?????? ???.
* ????????? ??????????? ???????????? ??? ? ??????????? ?? ?????????? ? ????
* ????????? ??????????? ?????? ?????? ?????? (?????????? ???) - ??????????? ?????? ?? ??????????? ????
*/

#include <QVector>
#include <map>

#include "Algo\combat.h";	// ??? ???????? LA

using std::vector;
using std::map;



// ??? ?????? ?????????: ?????????,   ????????????
enum ENGINE_TYPE { Fors = 0, UNFORS };
// ????? ??????: ???????????, ???????????????, ?????????
enum FLY_MODE	 { CREISER = 0, COMBIN, FORS };

struct PNVArea {
             // ?????,   ????????,   ?????????,   ??????????
    enum TYPE { NoneType = -1, Climb = 0, Cecline, Acceleration, Braking  };

    double h1, h2;			// ????????? ? ???????? ?????? ?? ??????? (????? ?????????)
    double v1, v2;			// ????????? ? ???????? ???????? ?? ??????? (????? ?????????)
    double vc;				// ????????? ?? ???????
    double vy;				// ???????????? ???????????? ????????

    /* ????????????? ?????????????? */
    double time;			// ????? ?????????? ??????? ???????
    double lenght;			// ????? ???? ??????? ???????

    TYPE type;					// { ?????, ????????, ?????????, ?????????? }
    ENGINE_TYPE eng_type;		// ????? ?????? ????????? ?? ?????? ???????


    PNVArea() {
        h1 = h2 = v1 = v2 -1;
        vc = vy = time = lenght = -1;
    }
};

PNVArea::TYPE getPnvAreaType(unsigned type);


typedef QVector<PNVArea*> PNV;

struct PNVInfo {
    double VN1, VN2, VN3, VN4;		// ??????????? ????????
    FLY_MODE fly_mode;				// ????? ??????
    double default_vc;				// ????????? ???????????, ???? ?? ???????? ?? ??????
    double default_vy;				// ???????????? ???????????? ????????, ???? ?? ???????? ?? ??????
    double default_dth;				// ???? ??????????? ??? ?????? ??????? ? ???????? (??? ??????? ?????)
    double dt;						// ????????? ???, ?????????? ??? ????????????

    double h_start, v_start;		// ????????? ?????? ? ???????? ???, ???? ?? ?????? ?????? ???????? ?? ?????? ??????? ???
    double theta, kurs;				// ?????? ?????????? ? ????
    double maxH;
    double time;                    // ????? ?????????? ???
    unsigned pnv_number;			// ????? ????????? ???

    QVector<PNVArea> areas;			// ?????????? ? ??????????? ????????
    QVector<PNV> pnvPath;			// ?????????? ? 12 ??? (?????????????????? ???????? ?? areas)


    PNVInfo() {
        default_vc = 3.0;
        default_dth = 1.0*d2r;

        dt = 1.0;
        h_start = 0;
        v_start = 100.0;
    }
    void createPath(unsigned number_pnv, QVector<unsigned> areaNumbers);
    void setVN(double vn1, double vn2, double vn3, double vn4);
    unsigned getAreaNumber(const PNVArea* area) const;
};

struct PNVPoint {
    double time;		// ????? ? ??????? ?????? ?????? ??????		[?]
    double height;		// ??????? ??????							[?]
    double x, z;        // ??????? ?????????? ? ?????????           [?]
    double lenght;		// ??????? ?????????? ?????????				[?]
    double v;			// ??????? ????????							[?/?]
    double g;			// ??????? ??????? ??????? (??????????????) [?]
    bool new_area;		// ??????? ?????? ?????? ??????? (?????? ??? ???????????)

    unsigned area;		// ????? ???????????? ???????				[1...24]
    PNVArea::TYPE type; // ??? ?????


    PNVPoint() { }
    PNVPoint(double t, double l, double h, double speed)
        : time(t), lenght(l), height(h), v(speed) {

        new_area = false;
    }
};


/* ?????? */


unsigned getNumberPnv(const PNVInfo& pnvInfo, const LA& our, const LA& target, FLY_MODE flying_mode);
void getRadius();

void integrateClimbArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track);
void integreateCeclineArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track);
void integrateAccelerationArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track);
void integrateBrakingArea(double& cur_h, double& cur_v, double h2, double v2, double vc, double vy, const PNVInfo& pnvInfo, QVector<PNVPoint>& track);

void integrateArea(const PNVArea* area, PNVInfo& pnvInfo, double& cv, double& ch, double th, QVector<PNVPoint>& track, const LA& target);
void buildPnvTrack(const LA& our, const LA& target, PNVInfo& pnvInfo, QVector<PNVPoint>& track, FLY_MODE flying_mode = CREISER);
