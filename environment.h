#ifndef NHF_ENVIRONMENT_H
#define NHF_ENVIRONMENT_H

#include "vector2.h"
#include "stdlib.h"
#include "stdbool.h"

typedef struct EnvironmentPoint {
    unsigned height;
    bool water;
} EnvironmentPoint;

typedef struct Environment {
    EnvironmentPoint **base;
    int w, h;
    int resolution;
} Environment;

Environment GenerateRandomEnvironment(int w, int h);
void FreeEnvironment(Environment* e);
EnvironmentPoint GetHeightAtCoordinates(Environment* e, int x, int y);


#endif //NHF_ENVIRONMENT_H
