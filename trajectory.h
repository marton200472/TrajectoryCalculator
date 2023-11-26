#ifndef NHF_TRAJECTORY_H
#define NHF_TRAJECTORY_H

#include "environment.h"
#include "artydata.h"
#include "point.h"

#define G 9.81
#define PI 3.14159265359
#define SIMULATION_TIMESTEP_S 2

typedef struct AngleResult {
    int resultCount;
    double results[2];
} AngleResult;



typedef struct ProjectilePoint {
    FPoint position;
    unsigned short color;
} ProjectilePoint;

typedef struct TrajectoryInfo {
    Point pointOfImpact;
    int pointCount;
    ProjectilePoint *points;
} TrajectoryInfo;





AngleResult FindVerticalAngleToTarget(double v0, double x, double y, ArtilleryData *artyData);

TrajectoryInfo CalculateTrajectory(Environment *env, Point *pos, double azimuth, double v0, double angle);

double RadToDeg(double rad);

double DegToRad(double deg);

Point GetProjectileMapLocationAtTime(Point artyPos, double azimuth, double t, double v0, double angle);

#endif //NHF_TRAJECTORY_H
