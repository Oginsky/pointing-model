// Переход от Гривичской геоцентрической правой прямоугольной СК (x,y,z)
// к геоцентрической сферической СК (0 долготы - гринвич) (r,f,l)
// EC earth-centered; SCS spherical coordinate system;
bool conversionGeoCartesToGeoSpheric(double x, double y, double z, double &r, double &f, double &l);

// Переход от геоцентрической сферической СК (0 долготы - гринвич) (r,f,l)
// к Гринвичской геоцентрической правой прямоугольной СК (X,Y,Z)
// (OХ - на Гринвич, OY - на Восток, OZ - на Север (по оси))
bool conversionGeoSphericToGeoCartes(double r, double f, double l, double &x, double &y, double &z);

// Переход от топоцентрической прямоугольной СК к абсолютной геоцентрической прямоугольной СК (топоцентрические координаты, абсолютные координаты, геодезические координаты центра ТСК)
void conversionTopoCartesToGeoCartes(double xt, double yt, double zt, double& xg, double& yg, double& zg, double r, double fc, double lc);

// Переход от топоцентрической прямоугольной СК к стартовой топоцентрической прямоугольной СК (топоцентрические координаты, стартовые координаты, азимут разворота)
// Стартовая СК получается поворотом топоцентрической СК на заданный угол относительно оси OY (угол, как правило, выбирается в соответствии с направлением старта или цели)
void conversionTopoCartesToStartCartes(double xt, double yt, double zt, double& xs, double& ys, double& zs, double a);

// Переход от абсолютной геоцентрической прямоугольной СК к топоцентрической прямоугольной СК (абсолютные координаты, топоцентрические координаты, геодезические координаты центра ТСК)
void conversionGeoCartesToTopoCartes(double xg, double yg, double zg, double& xt, double& yt, double& zt, double r, double fc, double lc);


// Переход от геоцентрической сферической СК к азимутальной равнодистантной СК на сфере
void projectionGeoSphericToAzimuthalDist_Sphere(double lat, double lon, double clat, double clon, double& x, double& y);
// Переход от азимутальной равнодистантной СК к геоцентрической сферической на сфере
void projectionAzimuthalDistToGeoSpheric_Sphere(double x, double y, double clat, double clon, double& lat, double& lon);

// Переход от геоцентрической сферической СК к азимутальной равнодистантной СК на эллипсоиде
void projectionGeoSphericToAzimuthalDist_Ellips(double lat, double lon, double clat, double clon, double& x, double& y);
// Переход от геоцентрической сферической СК к азимутальной равнодистантной СК на эллипсоиде
void projectionAzimuthalDistToGeoSpheric_Ellips(double x, double y, double clat, double clon, double& lat, double& lon);

// Переход от геоцентрической сферической СК к азимутальной равноугольной СК на сфере
void projectionGeoSphericToAzimuthalEquiAngle_Sphere(double lat, double lon, double clat, double clon, double& x, double& y);
// Переход от геоцентрической сферической СК к азимутальной равноугольной СК на сфере
void projectionAzimuthalEquiAngleToGeoSpheric_Sphere(double x, double y, double clat, double clon, double& lat, double& lon);

// Переход от геоцентрической сферической СК к азимутальной равноугольной СК на эллипсоиде
void projectionGeoSphericToAzimuthalEquiAngle_Ellips(double lat, double lon, double clat, double clon, double& x, double& y);
// Переход от геоцентрической сферической СК к азимутальной равноугольной СК на эллипсоиде
void projectionAzimuthalEquiAngleToGeoSpheric_Ellips(double x, double y, double clat, double clon, double& lat, double& lon);
