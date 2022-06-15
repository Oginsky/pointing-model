#include "Reader.h"

#include <QVector>
#include <qstringlist.h>

#include <QFile>
#include <QTextStream>

#include "../Algo/combat.h"
#include "../Algo/pnv.h"
#include "../Algo/pointingmodel.h"

#include "../Tools/gfunc.h"

#include <fstream>

using std::ofstream;

Reader::Reader(void) {

}


Reader::~Reader(void) {

}


bool Reader::read_angp(QString filename, QVector<LA> trace) {
	 QFile file(filename); 
	 if(!file.open(QIODevice::ReadOnly)) return false;
	 QTextStream fin(&file);

	 // Очищаем другую информацию
	 trace.clear();

	 QStringList line;

	 // Пропуск общий информации
	 fin.readLine();
	 fin.readLine();

	 // Формат чтения:
	 // 533: t[0] n[1] n[2] t2[3] x[4] y[5] z[6] vx[7] vy[8] vz[9] n[10] n[11] n[12] o/t[13] n[14] n[15] n[16]
	 // 5:   t[0] x[1] y[2] z[3] vx[4] vy[5] vz[6] x*[7] y*[8] z*[9] gm[10]

	 auto get = [&line](int index) { return line.at(index).toDouble(); };

	 while(!fin.atEnd()) {
		 line = fin.readLine().split(";");
		 if(line.size() != 11) return false;

		 double t = get(0), x = get(1), y = get(2), z = get(3), vx = get(4), vy = get(5), vz = get(6);
		 double _x = get(7), _z = get(9);
		 double gm = get(10);
		 double v = distance(vx, 0, vz), kurs = angleBetweenPoints(Point2D(0, 0), Point2D(_z, _x));


		 double lat, lon, r;										// координаты точки
		 conversionGeoCartesToGeoSpheric(x, y, z, r, lat, lon);

		 double clat, clon, cr;										// координаты КП
		 conversionGeoCartesToGeoSpheric(4.66066e+06, 2.90954e+06, 3.22528e+06, cr, clat, clon);


		 conversionGeoCartesToTopoCartes(vx, vy, vz, vx, vy, vz, 0.0, clat, clon);

		//conversionGeoCartesToTopoCartes(x, y, z, x, y, z, cr, clat, clon);

		 projectionGeoSphericToAzimuthalDist_Sphere(lat, lon, clat, clon, z, x);
		 y = r - cr; 


		 trace.push_back( LA(MotionParam(x, y, z, vx, vy, vz, t, v), AngleParam(kurs, gm)) );
	 }

	 file.close();
	 return true;
}

bool Reader::read_area(QString filename, PNVInfo& pnvInfo) {
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) return false;
	QTextStream fin(&file);
	
	pnvInfo.areas.clear();

	QString line;
	fin.readLine();		// Пропускаем хедеры
	while(!fin.atEnd()) {
		line = fin.readLine();
		QStringList values = line.split(";");
		if(values.size() != 8) return false;

		pnvInfo.areas.push_back(PNVArea());

        pnvInfo.areas.back().type = getPnvAreaType(values[1].replace(",", ".").toInt());
        if(!values[2].isEmpty()) pnvInfo.areas.back().h1 = values[2].replace(",", ".").toDouble()*1000.0;
        if(!values[3].isEmpty()) pnvInfo.areas.back().h2 = values[3].replace(",", ".").toDouble()*1000.0;
        if(!values[4].isEmpty()) pnvInfo.areas.back().v1 = values[4].replace(",", ".").toDouble();
        if(!values[5].isEmpty()) pnvInfo.areas.back().v2 = values[5].replace(",", ".").toDouble();
        if(!values[6].isEmpty()) pnvInfo.areas.back().vc = values[6].replace(",", ".").toDouble();
        if(!values[7].isEmpty()) pnvInfo.areas.back().vy = values[7].replace(",", ".").toDouble();
	}

	file.close();
	return true;
}

bool Reader::read_pnv(QString filename, PNVInfo& pnvInfo) {
	QFile file(filename);
	if(!file.open(QIODevice::ReadOnly)) return false;
	QTextStream fin(&file);
	

	QString line;
	fin.readLine();		// Пропускаем хедеры

	while(!fin.atEnd()) {
		line = fin.readLine();
		QStringList values = line.split(";");
		if(values.size() != 2) return false;

		unsigned pnv_number = values[0].toUInt();
		QStringList numbers = values[1].split(" ");
		QVector<unsigned> areaNumbers(numbers.size());
		for(size_t i = 0; i < numbers.size(); ++i) { 
			if(numbers[i] == "БУ") areaNumbers[i] = 0;
			else areaNumbers[i] = numbers[i].toUInt();
		}


		pnvInfo.createPath(pnv_number, areaNumbers);
	}

	file.close();
	return true;
}


bool Reader::read_model_area(QString filename, Model::ClimbProgramConsts& climbProgramConsts) {
    QFile file(filename);
       if(!file.open(QIODevice::ReadOnly)) return false;
       QTextStream fin(&file);

       climbProgramConsts.areas.clear();

       QString line;
       fin.readLine();		// Пропускаем хедеры
       while(!fin.atEnd()) {
           line = fin.readLine();
           QStringList values = line.split(";");
           if(values.size() != 8) return false;

           climbProgramConsts.areas.push_back(Model::ClimbSectionConsts());

           climbProgramConsts.areas.back().type = Model::getPnvAreaType(values[1].replace(",", ".").toInt());
           if(!values[2].isEmpty()) climbProgramConsts.areas.back().h1 = values[2].replace(",", ".").toDouble()*1000.0;
           else climbProgramConsts.areas.back().h1 - -1;

           if(!values[3].isEmpty()) climbProgramConsts.areas.back().h2 = values[3].replace(",", ".").toDouble()*1000.0;
           else climbProgramConsts.areas.back().h2 - -1;

           if(!values[4].isEmpty()) climbProgramConsts.areas.back().v1 = values[4].replace(",", ".").toDouble();
           else climbProgramConsts.areas.back().v1 - -1;

           if(!values[5].isEmpty()) climbProgramConsts.areas.back().v2 = values[5].replace(",", ".").toDouble();
           else climbProgramConsts.areas.back().v2 - -1;

           if(!values[6].isEmpty()) climbProgramConsts.areas.back().vc = values[6].replace(",", ".").toDouble();
           if(!values[7].isEmpty()) climbProgramConsts.areas.back().vy = values[7].replace(",", ".").toDouble();
       }

       file.close();
       return true;
}

bool Reader::read_model_pnv(QString filename, Model::ClimbProgramConsts& climbProgramConsts) {
    QFile file(filename);
        if(!file.open(QIODevice::ReadOnly)) return false;
        QTextStream fin(&file);


        QString line;
        fin.readLine();		// Пропускаем хедеры

        while(!fin.atEnd()) {
            line = fin.readLine();
            QStringList values = line.split(";");
            if(values.size() != 2) return false;

            unsigned pnv_number = values[0].toUInt();
            QStringList numbers = values[1].split(" ");
            QVector<unsigned> areaNumbers(numbers.size());
            for(size_t i = 0; i < numbers.size(); ++i) {
                if(numbers[i] == "БУ") areaNumbers[i] = 0;
                else areaNumbers[i] = numbers[i].toUInt();
            }


            climbProgramConsts.createPath(pnv_number, areaNumbers);
        }

        file.close();
        return true;
}

bool Reader::write_track(QString filename, const Model::Track& track) {
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;

    QTextStream fout(&file);


    for(const Model::motionParamLA& pos: track)
        fout << pos.X << " " << pos.Y << " " << pos.Z << "\n";

    file.close();
    return true;
}
