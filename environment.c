#include "environment.h"
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "noise.h"
#include "2darr.h"
#include "file.h"

#define env_res 25

Environment GenerateRandomEnvironment(int w, int h)
{

    Environment e = {.w = w, .h=h, .resolution=env_res};
    double **base1 = GenerateNoiseBase(w,h,500);
    double **base2 = GenerateNoiseBase(w,h,200);
    e.base = (EnvironmentPoint**)Allocate2DArr(w/env_res+1,h/env_res+1, sizeof(EnvironmentPoint));
    for (int i = 0; i <= w; i+=env_res) {
        for (int j = 0; j <= h; j+=env_res) {
            int pval = pow((GetPointValue(base1,i,j,500)*1.5 + GetPointValue(base2,i,j,200)*0.5)/2.,7)*2060-60;
            if(pval < 0)
                e.base[i/env_res][j/env_res] = (EnvironmentPoint){.height = 0, .water = true};
            else
                e.base[i/env_res][j/env_res] = (EnvironmentPoint){.height = pval, .water = false};
        }
    }

    Free2DArr((void**)base1,w/500+2);
    Free2DArr((void**)base2,w/200+2);

    return e;
}



EnvironmentPoint GetHeightAtCoordinates(Environment* e, int x, int y)
{
    //return pow((GetPointValue(e->lbase,x,y,e->lres)*e->lw + GetPointValue(e->sbase,x,y,e->sres)*e->sw)/(e->lw+e->sw),7)*2060-60;
    Vector2 point = {.x= x / (double)e->resolution, .y=y / (double)e->resolution};
    int tlx = x / e->resolution;
    int tly = y / e->resolution;

    double xweight = point.x - tlx;
    double yweight = point.y - tly;

    EnvironmentPoint dotProducts[4];
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            dotProducts[2*i+j] = e->base[tlx+i][tly+j];
        }
    }

    double r1 = Interpolate(dotProducts[0].height,dotProducts[1].height, yweight);
    double r2 = Interpolate(dotProducts[2].height,dotProducts[3].height, yweight);

    double w1 = Interpolate(dotProducts[0].water?1:0,dotProducts[1].water?1:0, yweight);
    double w2 = Interpolate(dotProducts[2].water?1:0,dotProducts[3].water?1:0, yweight);

    double val = Interpolate(r1,r2,xweight);
    bool water = Interpolate(w1, w2,xweight) > 0.6;


    return (EnvironmentPoint){.height = val, .water=water};
}



Environment LoadEnvironment(bool randomMap, bool saveMap, int mapPathIndex, int rW, int rH, char *argv[]) {
    Environment env;
    if(randomMap)
    {
        srand(time(NULL));
        env = GenerateRandomEnvironment(rW,rH);
        if(saveMap)
            WriteEnvToFile(&env,argv[mapPathIndex]);
    }
    else
    {
        env = ReadEnvFromFile(argv[mapPathIndex]);
    }
    return env;
}

void FreeEnvironment(Environment* e)
{
    Free2DArr((void**)e->base, e->w/e->resolution+1);
}
