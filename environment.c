#include "environment.h"
#include <math.h>
#include "noise.h"
#include "2darr.h"
#include "file.h"
#include "vector2.h"


#define ENVIRONMENT_RESOLUTION 25

/**
 * Véletlenszerű domborzatot generál
 * @param w a generálandó terület szélessége
 * @param h a generálandó terület hosszúsága
 * @return a generált domborzat egy Environment struktúrában
 * @author Márton
 */
Environment GenerateRandomEnvironment(int w, int h)
{
    Environment e = {.w = w, .h=h, .resolution=ENVIRONMENT_RESOLUTION};
    //két különböző felbontású zaj alap generálása
    double **base1 = GenerateNoiseBase(w,h,500);
    double **base2 = GenerateNoiseBase(w,h,200);

    e.base = (EnvironmentPoint**)Allocate2DArr(w / ENVIRONMENT_RESOLUTION + 1, h / ENVIRONMENT_RESOLUTION + 1, sizeof(EnvironmentPoint));

    //a zaj alapok összekeverése
    for (int i = 0; i <= w; i+=ENVIRONMENT_RESOLUTION) {
        for (int j = 0; j <= h; j+=ENVIRONMENT_RESOLUTION) {
            //magasság az adott pontban (így viszonylag "szép" domborzatot generál)
            int pval = (int)(pow((GetValueAtCoordinates(base1, i, j, 500) * 1.5 + GetValueAtCoordinates(base2, i, j, 200) * 0.5) / 2., 7) * 2060 - 60);
            if(pval < 0)
                e.base[i / ENVIRONMENT_RESOLUTION][j / ENVIRONMENT_RESOLUTION] = (EnvironmentPoint){.height = 0, .water = true};
            else
                e.base[i / ENVIRONMENT_RESOLUTION][j / ENVIRONMENT_RESOLUTION] = (EnvironmentPoint){.height = pval, .water = false};
        }
    }

    Free2DArr((void**)base1,w/500+2);
    Free2DArr((void**)base2,w/200+2);

    return e;
}


/**
 * Kiszámolja egy adott pont magasságát, és hogy víz van-e ott
 * @param e a domborzat, amiben keressük a pont tulajdonságait
 * @param x x koordináta
 * @param y y koordináta
 * @return az adott pont tulajdonságai egy EnvironmentPoint struktúrában
 * @author Márton
 */
EnvironmentPoint GetHeightAtCoordinates(Environment* e, int x, int y)
{
    //a pont helye a kis felbontású alapban
    Vector2 point = {.x= x / (double)e->resolution, .y=y / (double)e->resolution};
    //top left x és y koordináták az alapban
    int tlx = x / e->resolution;
    int tly = y / e->resolution;

    //horizontális és vertikális súlyok (a négy saroktól függően)
    double xweight = point.x - tlx;
    double yweight = point.y - tly;

    EnvironmentPoint corners[4];
    for (int i = 0; i <= 1; ++i) {
        for (int j = 0; j <= 1 ; ++j) {
            corners[2 * i + j] = e->base[tlx + i][tly + j];
        }
    }

    //a pont magasságának kiszámítása interpolációval
    double r1 = Interpolate(corners[0].height, corners[1].height, yweight);
    double r2 = Interpolate(corners[2].height, corners[3].height, yweight);

    double w1 = Interpolate(corners[0].water ? 1 : 0, corners[1].water ? 1 : 0, yweight);
    double w2 = Interpolate(corners[2].water ? 1 : 0, corners[3].water ? 1 : 0, yweight);

    double val = Interpolate(r1,r2,xweight);

    bool water = Interpolate(w1, w2,xweight) > 0.6;


    return (EnvironmentPoint){.height = (int)val, .water=water};
}


/**
 * Paraméterei alapján domborzatot generál vagy fájlból betölt
 * @param randomMap generált legyen-e a domborzat
 * @param saveMap el kell-e menteni a domborzatot
 * @param mapPathIndex a betöltéshez/mentéshez használt parancssori paraméter indexe
 * @param rW a generált domborzat szélessége
 * @param rH a generált domborzat hosszúsága
 * @param argv parancssori paraméterek
 * @return a betöltött/generált domborzat egy Environment struktúrában
 * @author Márton
 */
Environment LoadEnvironment(bool randomMap, bool saveMap, int mapPathIndex, int rW, int rH, char *argv[]) {
    Environment env;
    if(randomMap)
    {
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

/**
 * Felszabadítja egy Environment struktúra dinamikus memóriaterületét
 * @param e pointer a struktúrára
 * @author Márton
 */
void FreeEnvironment(Environment* e)
{
    Free2DArr((void**)e->base, e->w/e->resolution+1);
}
