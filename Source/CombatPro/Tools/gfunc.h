// ������� �� ���������� ��������������� ������ ������������� �� (x,y,z)
// � ��������������� ����������� �� (0 ������� - �������) (r,f,l)
// EC earth-centered; SCS spherical coordinate system;
bool conversionGeoCartesToGeoSpheric(double x, double y, double z, double &r, double &f, double &l);

// ������� �� ��������������� ����������� �� (0 ������� - �������) (r,f,l)
// � ����������� ��������������� ������ ������������� �� (X,Y,Z)
// (O� - �� �������, OY - �� ������, OZ - �� ����� (�� ���))
bool conversionGeoSphericToGeoCartes(double r, double f, double l, double &x, double &y, double &z);

// ������� �� ���������������� ������������� �� � ���������� ��������������� ������������� �� (���������������� ����������, ���������� ����������, ������������� ���������� ������ ���)
void conversionTopoCartesToGeoCartes(double xt, double yt, double zt, double& xg, double& yg, double& zg, double r, double fc, double lc);

// ������� �� ���������������� ������������� �� � ��������� ���������������� ������������� �� (���������������� ����������, ��������� ����������, ������ ���������)
// ��������� �� ���������� ��������� ���������������� �� �� �������� ���� ������������ ��� OY (����, ��� �������, ���������� � ������������ � ������������ ������ ��� ����)
void conversionTopoCartesToStartCartes(double xt, double yt, double zt, double& xs, double& ys, double& zs, double a);

// ������� �� ���������� ��������������� ������������� �� � ���������������� ������������� �� (���������� ����������, ���������������� ����������, ������������� ���������� ������ ���)
void conversionGeoCartesToTopoCartes(double xg, double yg, double zg, double& xt, double& yt, double& zt, double r, double fc, double lc);


// ������� �� ��������������� ����������� �� � ������������ ��������������� �� �� �����
void projectionGeoSphericToAzimuthalDist_Sphere(double lat, double lon, double clat, double clon, double& x, double& y);
// ������� �� ������������ ��������������� �� � ��������������� ����������� �� �����
void projectionAzimuthalDistToGeoSpheric_Sphere(double x, double y, double clat, double clon, double& lat, double& lon);

// ������� �� ��������������� ����������� �� � ������������ ��������������� �� �� ����������
void projectionGeoSphericToAzimuthalDist_Ellips(double lat, double lon, double clat, double clon, double& x, double& y);
// ������� �� ��������������� ����������� �� � ������������ ��������������� �� �� ����������
void projectionAzimuthalDistToGeoSpheric_Ellips(double x, double y, double clat, double clon, double& lat, double& lon);

// ������� �� ��������������� ����������� �� � ������������ ������������� �� �� �����
void projectionGeoSphericToAzimuthalEquiAngle_Sphere(double lat, double lon, double clat, double clon, double& x, double& y);
// ������� �� ��������������� ����������� �� � ������������ ������������� �� �� �����
void projectionAzimuthalEquiAngleToGeoSpheric_Sphere(double x, double y, double clat, double clon, double& lat, double& lon);

// ������� �� ��������������� ����������� �� � ������������ ������������� �� �� ����������
void projectionGeoSphericToAzimuthalEquiAngle_Ellips(double lat, double lon, double clat, double clon, double& x, double& y);
// ������� �� ��������������� ����������� �� � ������������ ������������� �� �� ����������
void projectionAzimuthalEquiAngleToGeoSpheric_Ellips(double x, double y, double clat, double clon, double& lat, double& lon);
