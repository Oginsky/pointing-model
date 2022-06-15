#pragma once

#include <QString>
#include <vector>


struct PNVInfo;
struct LA;

namespace Model {
    struct motionParamLA;
    typedef std::vector<motionParamLA> Track;

    struct ClimbProgramConsts;
}


class Reader {

public:
    Reader();
    ~Reader();

    static bool read_angp(QString filename, QVector<LA> trace);
    static bool read_area(QString filename, PNVInfo& pnvInfo);
    static bool read_pnv(QString filename, PNVInfo& pnvInfo);

    static bool read_model_area(QString filename, Model::ClimbProgramConsts& climbProgramConsts);
    static bool read_model_pnv(QString filename, Model::ClimbProgramConsts& climbProgramConsts);
    static bool write_track(QString filename, const Model::Track& track);

private:
};

