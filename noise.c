
#include "noise.h"
#include "2darr.h"
#include "vector2.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>



//smootherstep function suggested by Ken Perlin, https://en.wikipedia.org/wiki/Smoothstep#Variations
double Smootherstep(double x) {
    if(x<=0)
        return 0;
    if(x>=1)
        return 1;

    return 6*pow(x,5) - 15*pow(x,4) + 10* pow(x,3);
}







double Interpolate(double a, double b, double w) {
    return (b-a) * Smootherstep(w) + a;
}


double GetPointValue(double **base, int xpos, int ypos, int scale) {
    Vector2 point = {.x= xpos / (double)scale, .y=ypos / (double)scale};
    int tlx = xpos / scale;
    int tly = ypos / scale;

    double xweight = point.x - tlx;
    double yweight = point.y - tly;

    double dotProducts[4];
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            dotProducts[2*i+j] = base[tlx+i][tly+j];
        }
    }

    double r1 = Interpolate(dotProducts[0],dotProducts[1], yweight);
    double r2 = Interpolate(dotProducts[2],dotProducts[3], yweight);


    //return values between 0 and 1
    double val = Interpolate(r1,r2,xweight) * 0.5 + 0.5;


    return val;
}



double **GenerateNoiseBase(int w, int h, int scale) {
    int gxdim = w / scale + 2;
    int gydim = h / scale + 2;

    double ** grid = (double**)Allocate2DArr(gxdim, gydim, sizeof(double));
    double min = 1;
    for (int i = 0; i < gxdim; ++i) {
        for (int j = 0; j < gydim; ++j) {
            grid[i][j] = (double)rand()/(double )RAND_MAX;
            if (grid[i][j] < min)
                min = grid[i][j];
        }
    }
    printf("%lf\n",min);

    return grid;
}





