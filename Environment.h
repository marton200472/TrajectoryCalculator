#ifndef NHF_ENVIRONMENT_H
#define NHF_ENVIRONMENT_H

typedef struct Environment {
    double **lbase, **sbase;
    int w, h;
    int lres, sref;
} Environment;

int** GetEnvironmentSection(int x, int y, int w, int h);

int** GenerateRandomEnvironment(int w, int h);


#endif //NHF_ENVIRONMENT_H
