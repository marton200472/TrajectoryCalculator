#include "trajectory.h"
#include <math.h>

#define G 9.81
#define PI 3.14159265359

double RadToDeg(double rad){
    return rad * 180 / PI;
}

//https://en.wikipedia.org/wiki/Projectile_motion#Angle_%CE%B8_required_to_hit_coordinate_(x,_y)
AngleResult FindVerticalAngleToTarget(double v0, double x, double y) {
    double D = pow(v0,4)-G*(G*pow(x,2)+2*y*pow(v0,2));
    if (D<0)
        return (AngleResult){.success=false};

    double res1 = atan((pow(v0,2) + sqrt( D )) / (G*x));
    double res2 = atan((pow(v0,2) - sqrt( D )) / (G*x));


    return (AngleResult){true, RadToDeg(res1), RadToDeg(res2)};
}


