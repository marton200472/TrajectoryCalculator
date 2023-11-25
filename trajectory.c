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

FPoint IntersectWithLine(double c, bool xc, FPoint basePoint, FPoint dirVec)
{
    if (xc)
        return (FPoint){.x=c, .y=(dirVec.y*basePoint.x - dirVec.x*basePoint.y - dirVec.y*c)/-dirVec.x};
    else
        return (FPoint){.x=(dirVec.y*basePoint.x - dirVec.x*basePoint.y + dirVec.x*c)/dirVec.y, .y=c};
}



double GetYFromX(double x, double angle, double v0) {
    return tan(angle)*x - G/(2*v0*v0*pow(cos(angle),2))*x*x;
}

FPoint GetDirectionVector(double azimuth)
{
    return (FPoint){.y=-cos(azimuth), .x=sin(azimuth)};

}



Point GetProjectileLocationAtTime(Point artyPos, double azimuth, double t, double v0, double angle) {
    FPoint dirVec = GetDirectionVector(azimuth);
    double dst = DistanceAtTime(angle,v0,t)/10;
    return (Point){(int)(artyPos.x+ dst*dirVec.x), (int)(artyPos.y + dst*dirVec.y)};
}


TrajectoryInfo CalculateTrajectory(Environment *env, Point *pos, double azimuth, double v0, double angle) {
    FPoint fPos = {pos->x+0.5,pos->y+0.5};
    double azRad = DegToRad(azimuth);
    FPoint dirVec = GetDirectionVector(azRad);
    int hStep = dirVec.x > 0 ? 1 : -1;
    int vStep = dirVec.y > 0 ? 1 : -1;
    int hLine=hStep==-1?pos->x:pos->x+1, vLine=vStep==-1?pos->y:pos->y+1;
    TrajectoryInfo trInfo = {{-1,-1}};
    while (true) {
        FPoint vIntersect = IntersectWithLine(vLine,false,fPos,dirVec);
        FPoint hIntersect = IntersectWithLine(hLine,true,fPos,dirVec);
        double vDst = sqrt(pow(vIntersect.x-fPos.x,2)+pow(vIntersect.y-fPos.y,2))*10;
        double hDst = sqrt(pow(hIntersect.x-fPos.x,2)+pow(hIntersect.y-fPos.y,2))*10;

        Point p1, p2;
        double h;
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
            trInfo.pointOfImpact = p1;
            break;
        }
        else if(GetHeightAtCoordinates(env,p2.x,p2.y).height >= h)
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

    double dstToTarget = sqrt(pow(trInfo.pointOfImpact.x-fPos.x,2)+pow(trInfo.pointOfImpact.y-fPos.y,2))*10;
    double tToTarget = dstToTarget / (cos(DegToRad(angle)) * v0);
    double t = SIMULATION_TIMESTEP_S;
    double maxH = GetHeightAtCoordinates(env, pos->x, pos->y).height + 1 + v0 * v0 * pow(sin(DegToRad(angle)), 2) / (2 * G);
    double minH = min(GetHeightAtCoordinates(env, pos->x, pos->y).height+1, GetHeightAtCoordinates(env, (int)trInfo.pointOfImpact.x, (int)trInfo.pointOfImpact.y).height);
    trInfo.pointCount = (int)(tToTarget / SIMULATION_TIMESTEP_S);
    trInfo.points = malloc(trInfo.pointCount*sizeof(ProjectilePoint));
    int i = 0;
    while (t<tToTarget)
    {
        double dst = DistanceAtTime(DegToRad(angle), v0, t)/10;
        double h = GetHeightAtCoordinates(env, pos->x, pos->y).height+1 +HeightAtTime(DegToRad(angle), v0, t);
        unsigned short color = 255-(unsigned short)(255 * (h-minH) / (maxH - minH));
        trInfo.points[i++] = (ProjectilePoint){{(fPos.x+ dirVec.x * dst), (fPos.y+dirVec.y * dst)}, color};
        t+=SIMULATION_TIMESTEP_S;
    }



    return trInfo;
}






