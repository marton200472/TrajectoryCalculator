#ifndef NHF_ENVIRONMENT_H
#define NHF_ENVIRONMENT_H

#include "vector2.h"

typedef struct Environment {
    double **lbase, **sbase;
    int w, h;
    int lres, sres;
    double lw, sw;
} Environment;

Environment GenerateRandomEnvironment(int w, int h);
void FreeEnvironment(Environment* e);
int GetHeightAtCoordinates(Environment* e, int x, int y);


#endif //NHF_ENVIRONMENT_H
