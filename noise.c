
#include "noise.h"
#include "2darr.h"
#include "vector2.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>



/**
 * Lineárisnál jobb interpolációhoz használható szorzót generál
 * kredit: Ken Perlin lsd.: https://en.wikipedia.org/wiki/Smoothstep#Variations
 * @param x 0 és 1 közti szám
 * @return 0 és 1 közti szám
 * @author Márton
 */
double Smootherstep(double x) {
    if(x<=0)
        return 0;
    if(x>=1)
        return 1;

    return 6*pow(x,5) - 15*pow(x,4) + 10* pow(x,3);
}

/**
 * Interpoláció két pont között a Smootherstep fv. segítségével
 * @param a a bal oldali pont
 * @param b a jobb oldali pont
 * @param w a távolság a bal oldali ponttól
 * @return az interpoláció eredménye (a és b közötti szám)
 * @author Márton
 */
double Interpolate(double a, double b, double w) {
    return (b-a) * Smootherstep(w) + a;
}

/**
 * Egy adott pontban kiszámolja a zaj értékét
 * @param base a zaj alacsony felbontású alapja
 * @param xpos x koordináta
 * @param ypos y koordináta
 * @param scale a zaj alap felbontása
 * @return a zaj értéke az adott pontban (0<=x<=1)
 * @author Márton
 */
double GetValueAtCoordinates(double **base, int xpos, int ypos, int scale) {
    //valódi pozíció
    Vector2 point = {.x= xpos / (double)scale, .y=ypos / (double)scale};
    int tlx = xpos / scale;
    int tly = ypos / scale;

    //függőleges és vízszintes súlyok
    double xweight = point.x - tlx;
    double yweight = point.y - tly;

    double corners[4];
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            corners[2 * i + j] = base[tlx + i][tly + j];
        }
    }


    double r1 = Interpolate(corners[0], corners[1], yweight);
    double r2 = Interpolate(corners[2], corners[3], yweight);


    //0 és 1 közti érték kialakítása
    double val = Interpolate(r1,r2,xweight) * 0.5 + 0.5;


    return val;
}


/**
 * Zaj alapot generál
 * @param w szélesség
 * @param h hosszúság
 * @param scale felbontás
 * @return a generált, dinamikusan foglalt 2D-s double tömb
 * @author Márton
 */
double **GenerateNoiseBase(int w, int h, int scale) {
    int gxdim = w / scale + 2;
    int gydim = h / scale + 2;

    double ** grid = (double**)Allocate2DArr(gxdim, gydim, sizeof(double));
    for (int i = 0; i < gxdim; ++i) {
        for (int j = 0; j < gydim; ++j) {
            //0 és 1 közti double generálása
            grid[i][j] = (double)rand()/(double)RAND_MAX;
        }
    }

    return grid;
}





