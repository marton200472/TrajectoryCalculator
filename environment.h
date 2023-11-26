#ifndef NHF_ENVIRONMENT_H
#define NHF_ENVIRONMENT_H


#include <stdbool.h>

typedef struct EnvironmentPoint {
    int height;
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

Environment LoadEnvironment(bool randomMap, bool saveMap, int mapPathIndex, int rW, int rH, char *argv[]);



#endif //NHF_ENVIRONMENT_H
