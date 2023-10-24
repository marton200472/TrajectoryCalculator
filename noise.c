//Perlin noise generator, based on https://en.wikipedia.org/wiki/Perlin_noise#Algorithm_detail
#include "noise.h"
#include "2darr.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>




double Smootherstep(double x) {
    if(x<=0)
        return 0;
    if(x>=1)
        return 1;

    return 6*pow(x,5) - 15*pow(x,4) + 10* pow(x,3);
}

typedef struct Vector2 {
    double x, y;
} Vector2;

Vector2 RandomVector()
{
    double r=(2*M_PI/RAND_MAX) * rand();
    return (Vector2){.x=cos(r), .y=sin(r)};
}

double DotProduct(Vector2 a, Vector2 b)
{
    return a.x*b.x+a.y*b.y;
}


Vector2 OffsetVector(Vector2 from, Vector2 to)
{
    return ((Vector2 ){to.x - from.x, to.y - from.y});
}

double Interpolate(double a, double b, double w)
{
    return (b-a) * Smootherstep(w) + a;
}

double GetPointValue(Vector2** base, int xpos, int ypos, int scale)
{
    Vector2 point = {.x= xpos / (double)scale, .y=ypos / (double)scale};
    int tlx = xpos / scale;
    int tly = ypos / scale;

    double xweight = point.x - tlx;
    double yweight = point.y - tly;

    double dotProducts[4];
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            Vector2 offset = OffsetVector((Vector2){tlx+i,tly+j}, point);
            dotProducts[2*i+j] = DotProduct(offset, base[tlx+i][tly+j]);
        }
    }

    double r1 = Interpolate(dotProducts[0],dotProducts[1], yweight);
    double r2 = Interpolate(dotProducts[2],dotProducts[3], yweight);


    //return values between 0 and 1
    double val = Interpolate(r1,r2,xweight) * 0.5 + 0.5;


    return val;
}


double **GeneratePerlinNoise(int w, int h, int scale) {
    int gxdim = w / scale + 2;
    int gydim = h / scale + 2;

    Vector2** grid = Allocate2DArr(gxdim, gydim, sizeof(Vector2));
    for (int i = 0; i < gxdim; ++i) {
        for (int j = 0; j < gydim; ++j) {
            grid[i][j] = RandomVector();
        }
    }

    double** noise = Allocate2DArr(w,h,sizeof(double));
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {

            noise[i][j] = GetPointValue(grid, i,j,scale);
        }
    }
    Free2DArr(grid,gxdim);
    return noise;
}

