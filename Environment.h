#ifndef NHF_ENVIRONMENT_H
#define NHF_ENVIRONMENT_H

#include "vector2.h"

typedef struct Environment {
    double **lbase, **sbase;
    int w, h;
    int lres, sres;
    double lw, sw;
} Environment;

int** GenerateRandomEnvironment(int w, int h);


#endif //NHF_ENVIRONMENT_H
