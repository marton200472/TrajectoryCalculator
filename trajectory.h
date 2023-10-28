#ifndef NHF_TRAJECTORY_H
#define NHF_TRAJECTORY_H

#include <stdbool.h>

typedef struct AngleResult {
    bool success;
    double result1, result2;
} AngleResult;



AngleResult FindVerticalAngleToTarget(double v0, double x, double y);

#endif //NHF_TRAJECTORY_H
