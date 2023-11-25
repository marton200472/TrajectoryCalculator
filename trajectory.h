#ifndef NHF_TRAJECTORY_H
#define NHF_TRAJECTORY_H

#include <stdbool.h>
#include <math.h>
#include "environment.h"
#include "minmax.h"

#define G 9.81
#define PI 3.14159265359
#define SIMULATION_TIMESTEP_S 2

typedef struct AngleResult {
    int resultCount;
    double results[2];
} AngleResult;

typedef struct Point {
    int x, y;
} Point;

typedef struct FPoint{
    double x, y;
} FPoint;

typedef struct ProjectilePoint {
    FPoint position;
    unsigned short color;
} ProjectilePoint;

typedef struct TrajectoryInfo {
    Point pointOfImpact;
    int pointCount;
    ProjectilePoint *points;
} TrajectoryInfo;





AngleResult FindVerticalAngleToTarget(double v0, double x, double y);

TrajectoryInfo CalculateTrajectory(Environment *env, Point *pos, double azimuth, double v0, double angle);

double RadToDeg(double rad);

double DegToRad(double deg);

Point GetProjectileLocationAtTime(Point artyPos,double azimuth,double t, double v0, double angle);

#endif //NHF_TRAJECTORY_H
