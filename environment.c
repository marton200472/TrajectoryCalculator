#include "environment.h"
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "noise.h"
#include "2darr.h"



//TODO multithreading
Environment GenerateRandomEnvironment(int w, int h)
{

    Environment e;
    e.lbase = GenerateNoiseBase(w,h,500);
    e.sbase = GenerateNoiseBase(w,h,200);
    e.w = w;
    e.h = h;
    e.lres = 500;
    e.sres = 200;
    e.lw = 1.5;
    e.sw = 0.5;

    return e;
}



int GetHeightAtCoordinates(Environment* e, int x, int y)
{
    return pow((GetPointValue(e->lbase,x,y,e->lres)*e->lw + GetPointValue(e->sbase,x,y,e->sres)*e->sw)/(e->lw+e->sw),7)*2060-60;
}

void FreeEnvironment(Environment* e)
{
    Free2DArr(e->lbase, e->w/e->lres+2);
    Free2DArr(e->sbase, e->w/e->sres+2);
}
