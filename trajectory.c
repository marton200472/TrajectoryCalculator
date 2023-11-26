#include "trajectory.h"
#include "debugmalloc.h"

/**
 * Radiánból fokba konvertál
 * @param rad a konvertálandó érték radiánban
 * @return a konvertált érték fokban
 * @author Márton
 */
double RadToDeg(double rad) {
    return rad * 180 / PI;
}

/**
 * Fokból radiánba konvertál
 * @param deg a konvertálandó érték fokban
 * @return a konvertált érték radiánban
 * @author Márton
 */
double DegToRad(double deg) {
    return deg * PI / 180;
}

/**
 * Kiszámolja egy adott pont eltalálásához szükséges szöget adott kezdősebesség esetén
 * lsd.: https://en.wikipedia.org/wiki/Projectile_motion#Angle_%CE%B8_required_to_hit_coordinate_(x,_y)
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param x a cél x koordiátája
 * @param y a cél y koordinátája
 * @param artyData a löveg adatai
 * @return a lehetséges szögeket tartalmazó struktúra
 * @author Márton
 */
AngleResult FindVerticalAngleToTarget(double v0, double x, double y, ArtilleryData *artyData) {
    //diszkrimináns
    double D = pow(v0,4)-G*(G*pow(x,2)+2*y*pow(v0,2));
    AngleResult ar = {.resultCount=0};
    if (D<0)
        return ar;

    double res1 = RadToDeg(atan((pow(v0,2) + sqrt( D )) / (G*x)));
    double res2 = RadToDeg(atan((pow(v0,2) - sqrt( D )) / (G*x)));

    if (res1 >= artyData->minAngle && res1 <= artyData->maxAngle)
        ar.results[ar.resultCount++] = res1;

    if(D > 0 && res2 >= artyData->minAngle && res2 <= artyData->maxAngle)
        ar.results[ar.resultCount++] = res2;

    return ar;
}


/**
 * Kiszámolja a lövedék kezdőponthoz relatív magasságát egy adott pillanatban
 * @param angle lövés leadásának szöge radiánban
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param t az eltelt idő s-ban
 * @return a relatív magasság m-ben
 * @author Márton
 */
double HeightAtTime(double angle, double v0, double t) {
    return v0*t*sin(angle) - 0.5*G*t*t;
}

/**
 * Kiszámolja a lövedék kezdőponthoz relatív távolságának vízszintes komponensét egy adott pillanatban
 * @param angle lövés leadásának szöge radiánban
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param t az eltelt idő s-ban
 * @return a relatív távolság vízszintes komponense m-ben
 * @author Márton
 */
double DistanceAtTime(double angle, double v0, double t) {
    return v0*t*cos(angle);
}

/**
 * Adott pontból induló, adott irányvektorú egyenes metszéspontját számolja ki egy vízszintes vagy függőleges egyenessel
 * @param lineCoord az egyik egyenes x vagy y koordinátája (x ha függőleges, y ha vízszintes)
 * @param verticalLine függőleges egyenessel számolunk?
 * @param basePoint a másik egyenes kezdőpontja
 * @param dirVec a másik egyenes irányvektora
 * @return a metszéspont koordinátái
 * @author Márton
 */
FPoint IntersectWithLine(double lineCoord, bool verticalLine, FPoint basePoint, Vector2 dirVec)
{
    if (verticalLine)
        return (FPoint){.x=lineCoord, .y=(dirVec.y * basePoint.x - dirVec.x * basePoint.y - dirVec.y * lineCoord) / -dirVec.x};
    else
        return (FPoint){.x=(dirVec.y*basePoint.x - dirVec.x*basePoint.y + dirVec.x * lineCoord) / dirVec.y, .y=lineCoord};
}


/**
 * Távolság vízszintes komponenséhez függőlegeset rendel
 * @param x a távolság vízszintes komponense
 * @param angle a lövés leadásának szöge
 * @param angle a lövedék kezdősebessége m/s-ban
 * @return
 * @author Márton
 */
double GetYFromX(double x, double angle, double v0) {
    return tan(angle)*x - G/(2*v0*v0*pow(cos(angle),2))*x*x;
}

/**
 * Kiszámolja a lövedék vízszintes irányvektorát az azimutból (irányszög)
 * @param azimuth a lövés azimutja radiánban
 * @return a lövedék vízszintes irányvektora, egység hosszúságú
 * @author Márton
 */
Vector2 GetDirectionVector(double azimuth)
{
    return (Vector2){.y=-cos(azimuth), .x=sin(azimuth)};

}


/**
 * Kiszámolja a lövedék pozícióját a teljes térképhez viszonyítva
 * @param artyPos a löveg pozíciója
 * @param azimuth a löveg azimutja (irányszöge) radiánban
 * @param t az eltelt idő s-ban
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param angle a lövés szöge radiánban
 * @return a lövedék pozíciója
 * @author Márton
 */
Point GetProjectileMapLocationAtTime(Point artyPos,double azimuth, double t, double v0, double angle) {
    Vector2 dirVec = GetDirectionVector(azimuth);
    double dst = DistanceAtTime(angle,v0,t)/10;
    return (Point){(int)(artyPos.x+ dst*dirVec.x), (int)(artyPos.y + dst*dirVec.y)};
}


/**
 * kiszámolja a lövedék becsapódási helyét
 * @param env A domborzat, amivel számolunk
 * @param pos a löveg pozíciója
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param angle a lövés szöge fokban
 * @param fPos löveg pozíciója az adott négyzet közepén
 * @param dirVec a lövedék vízszintes irányvektora
 * @return a becsapódás helye
 * @author Márton
 */
Point CalculateImpactPoint(Environment *env, Point *pos, FPoint fPos, Vector2 dirVec, double angle, double v0)
{

    //a horizontális és a vertikális lépésköz
    int hStep = dirVec.x > 0 ? 1 : -1;
    int vStep = dirVec.y > 0 ? 1 : -1;
    //horizontális és vertikális kezdőegyenes koordinátája
    int hLine=hStep==-1?pos->x:pos->x+1, vLine=vStep==-1?pos->y:pos->y+1;


    while (true) {
        //metszéspontok és kezdőponttól való távolságok kiszámítása
        FPoint vIntersect = IntersectWithLine(vLine,false,fPos,dirVec);
        FPoint hIntersect = IntersectWithLine(hLine,true,fPos,dirVec);
        double vDst = sqrt(pow(vIntersect.x-fPos.x,2)+pow(vIntersect.y-fPos.y,2))*10;
        double hDst = sqrt(pow(hIntersect.x-fPos.x,2)+pow(hIntersect.y-fPos.y,2))*10;

        Point p1, p2;
        double h;
        //a közelebbi metszésponttal kell számolni
        //a lövedék kezdőmagasságát magasság a kezdőpozícióban+1-nek vettem
        if (hDst < vDst)
        {
            if (hIntersect.x < 0 || hIntersect.x >= env->w || hIntersect.y < 0 || hIntersect.y >= env->h)
                break;
            p1 = (Point){(int)hIntersect.x, (int)hIntersect.y};
            p2 = (Point){p1.x-1,p1.y};
            h = GetHeightAtCoordinates(env, pos->x, pos->y).height + 1 + GetYFromX(hDst, DegToRad(angle),v0);
            hLine += hStep;
        }
        else
        {
            if (vIntersect.x < 0 || vIntersect.x >= env->w || vIntersect.y < 0 || vIntersect.y >= env->h)
                break;
            p1 = (Point){(int)vIntersect.x, (int)vIntersect.y};
            p2 = (Point){p1.x,p1.y-1};
            h = GetHeightAtCoordinates(env, pos->x, pos->y).height + 1 + GetYFromX(vDst, DegToRad(angle),v0);
            vLine += vStep;
        }

        if (GetHeightAtCoordinates(env,p1.x,p1.y).height >= h)
        {
            return p1;
        }
        else if(GetHeightAtCoordinates(env,p2.x,p2.y).height >= h)
        {
            return p2;
        }
    }
    return (Point){-1,-1};
}

/**
 * Kiszámolja a lövedék néhány pozícióját, és dinamikus memóriában eltárolja a paraméterként kapott pointeren
 * @param points a pointer címe, ahova az adatok kerülnek
 * @param env a domborzat adatai
 * @param pos a löveg pozíciója
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param angle a lövés szöge fokban
 * @param fPos löveg pozíciója az adott négyzet közepén
 * @param dirVec a lövedék vízszintes irányvektora
 * @return a dinamikusan foglalt memória mérete
 * @author Márton
 */
int CalculateProjectilePositions(ProjectilePoint **points,Environment *env,Point pointOfImpact,Point *pos,FPoint fPos,Vector2 dirVec,double angle, double v0)
{
    double dstToTarget = sqrt(pow(pointOfImpact.x-fPos.x,2)+pow(pointOfImpact.y-fPos.y,2))*10;
    double tToTarget = dstToTarget / (cos(DegToRad(angle)) * v0);
    double t = SIMULATION_TIMESTEP_S;
    double maxH = GetHeightAtCoordinates(env, pos->x, pos->y).height + 1 + v0 * v0 * pow(sin(DegToRad(angle)), 2) / (2 * G);
    double minH = min(GetHeightAtCoordinates(env, pos->x, pos->y).height+1, GetHeightAtCoordinates(env, (int)pointOfImpact.x, (int)pointOfImpact.y).height);
    int pointCount = (int)(tToTarget / SIMULATION_TIMESTEP_S);
    *points = malloc(pointCount*sizeof(ProjectilePoint));
    int i = 0;
    //lövedék pozícióinak kiszámítása
    while (t<tToTarget)
    {
        double dst = DistanceAtTime(DegToRad(angle), v0, t)/10;
        double h = GetHeightAtCoordinates(env, pos->x, pos->y).height+1 +HeightAtTime(DegToRad(angle), v0, t);
        unsigned short color = 255-(unsigned short)(255 * (h-minH) / (maxH - minH));
        (*points)[i++] = (ProjectilePoint){{(fPos.x+ dirVec.x * dst), (fPos.y+dirVec.y * dst)}, color};
        t+=SIMULATION_TIMESTEP_S;
    }
    return pointCount;
}

/**
 * Kiszámolja a lövedék becsapódási pozícióját és a lövedék pozícióját SIMULATION_TIMESTEP_S s-onként
 * @param env a domborzat adatai
 * @param pos a löveg pozíciója
 * @param azimuth a löveg azimutja (irányszöge) fokban
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param angle a lövés szöge fokban
 * @return a kiszámolt adatok TrajectoryInfo struktúrában
 * @author Márton
 */
TrajectoryInfo CalculateTrajectory(Environment *env, Point *pos, double azimuth, double v0, double angle) {
    //a löveg pozíció négyzetének közepe
    FPoint fPos = {pos->x+0.5,pos->y+0.5};
    //azimut radiánban
    double azRad = DegToRad(azimuth);
    Vector2 dirVec = GetDirectionVector(azRad);

    TrajectoryInfo trInfo;

    trInfo.pointOfImpact = CalculateImpactPoint(env,pos,fPos,dirVec,angle,v0);

    if (trInfo.pointOfImpact.x == -1 || trInfo.pointOfImpact.y ==-1)
    {
        trInfo.pointCount = 0;
        trInfo.points = NULL;
        return trInfo;
    }

    trInfo.pointCount = CalculateProjectilePositions(&trInfo.points,env,trInfo.pointOfImpact,pos,fPos,dirVec,angle,v0);



    return trInfo;
}






