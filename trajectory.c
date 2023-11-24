#include "trajectory.h"
#include "debugmalloc.h"


double RadToDeg(double rad) {
    return rad * 180 / PI;
}

double DegToRad(double deg) {
    return deg * PI / 180;
}

//https://en.wikipedia.org/wiki/Projectile_motion#Angle_%CE%B8_required_to_hit_coordinate_(x,_y)
AngleResult FindVerticalAngleToTarget(double v0, double x, double y) {
    double D = pow(v0,4)-G*(G*pow(x,2)+2*y*pow(v0,2));
    if (D<0)
        return (AngleResult){.resultCount=0};

    double res1 = atan((pow(v0,2) + sqrt( D )) / (G*x));

    if (D == 0)
        return (AngleResult){.resultCount = 1, {RadToDeg(res1)}};

    double res2 = atan((pow(v0,2) - sqrt( D )) / (G*x));


    return (AngleResult){2, {RadToDeg(res1), RadToDeg(res2)}};
}



double HeightAtTime(double angle, double v0, double t) {
    return v0*t*sin(angle) - 0.5*G*t*t;
}

double DistanceAtTime(double angle, double v0, double t) {
    return v0*t*cos(angle);
}

FPoint IntersectWithLine(double c, bool xc, Point basePoint, FPoint dirVec)
{
    if (xc)
        return (FPoint){.x=c, .y=(dirVec.y*basePoint.x - dirVec.x*basePoint.y - dirVec.y*c)/-dirVec.x};
    else
        return (FPoint){.x=(dirVec.y*basePoint.x - dirVec.x*basePoint.y + dirVec.x*c)/dirVec.y, .y=c};
}



double GetYFromX(double x, double angle, double v0) {
    return tan(angle)*x - G/(2*v0*v0*pow(cos(angle),2))*x*x;
}



TrajectoryInfo CalculateTrajectory(Environment *env, Point *pos, double azimuth, double v0, double angle) {
    double azRad = DegToRad(azimuth);
    FPoint dirVec = {.y=-cos(azRad), .x=sin(azRad)};
    bool vertical = azimuth > 315 || azimuth <= 45 || (azimuth > 135 && azimuth <= 225);
    int step = (vertical && dirVec.y > 0) || (!vertical && dirVec.x > 0) ? 1 : -1;
    TrajectoryInfo trInfo = {{-1,-1}};
    for (int i = (int)(vertical?pos->y:pos->x) ; i >= 0 && i<(vertical?env->h:env->w); i+=step) {
        FPoint intersection = IntersectWithLine(i,!vertical,*pos,dirVec);
        Point p1 = {(int)intersection.x, (int)intersection.y};
        Point p2;
        if (vertical)
            p2 = (Point){p1.x, p1.y-1};
        else
            p2 = (Point){p1.x-1,p1.y};

        double diff = sqrt(pow(intersection.x-pos->x,2)+pow(intersection.y-pos->y,2))*10;
        double h = GetHeightAtCoordinates(env, (int)pos->x, (int)pos->y).height + 2 + GetYFromX(diff, DegToRad(angle),v0);

        if (GetHeightAtCoordinates(env,(int)p1.x,(int)p1.y).height >= h)
        {
            trInfo.pointOfImpact = p1;
            break;
        }
        else if(GetHeightAtCoordinates(env,(int)p2.x,(int)p2.y).height >= h)
        {
            trInfo.pointOfImpact = p2;
            break;
        }
    }

    if (trInfo.pointOfImpact.x == -1 || trInfo.pointOfImpact.y ==-1)
    {
        trInfo.pointCount = 0;
        trInfo.points = NULL;
        return trInfo;
    }

    double tStep = 2;
    double dstToTarget = sqrt(pow(trInfo.pointOfImpact.x-pos->x,2)+pow(trInfo.pointOfImpact.y-pos->y,2))*10;
    double tToTarget = dstToTarget / (cos(DegToRad(angle)) * v0);
    double t = tStep;
    double maxH = GetHeightAtCoordinates(env, (int)pos->x, (int)pos->y).height + 1 + v0 * v0 * pow(sin(DegToRad(angle)), 2) / (2 * G);
    double minH = min(GetHeightAtCoordinates(env, (int)pos->x, (int)pos->y).height, GetHeightAtCoordinates(env, (int)trInfo.pointOfImpact.x, (int)trInfo.pointOfImpact.y).height);
    trInfo.pointCount = (int)(tToTarget / tStep);
    trInfo.points = malloc(trInfo.pointCount*sizeof(ProjectilePoint));
    int i = 0;
    while (t<tToTarget)
    {
        double dst = DistanceAtTime(DegToRad(angle), v0, t)/10;
        trInfo.points[i++] = (ProjectilePoint){{(pos->x+ dirVec.x * dst), (pos->y+dirVec.y * dst)}, 255-(unsigned short)(255 * (HeightAtTime(
                DegToRad(angle), v0, t)-minH) / (maxH - minH))};
        t+=tStep;
    }



    return trInfo;
}






