#include "file.h"
#include "environment.h"
#include <stdio.h>
#include <string.h>
#include "2darr.h"

/**
 * Beolvassa egy tüzérségi löveg tulajdonságait
 * @param filename az adatokat tartalmazó fájl elérési útvonala
 * @return az adatokat tartalmazó ArtilleryData struktúra
 * @author Márton
 */
ArtilleryData ReadArtilleryData(const char *filename) {
    FILE* f = fopen(filename, "r");
    ArtilleryData ard;
    fscanf(f,"%lf", &ard.minAngle);
    fscanf(f,"%lf", &ard.maxAngle);
    fscanf(f,"%d",&ard.optionCount);
    ard.options = malloc(ard.optionCount * sizeof(double));
    for (int i = 0; i < ard.optionCount; ++i) {
        fscanf(f, "%lf", ard.options + i);
    }
    fclose(f);
    return ard;
}


/**
 * Domborzatot ment fájlba
 * @param e pointer a domborzatra
 * @param fileName a cél fájl elérési útvonala
 * @author Márton
 */
void WriteEnvToFile(Environment *e, const char *fileName) {
    FILE* f = fopen(fileName, "w");
    fprintf(f,"%d %d\n",e->w, e->h);
    fprintf(f,"%d\n",e->resolution);
    for (int i = 0; i <= e->h; i+=e->resolution) {
        for (int j = 0; j <= e->w; j+=e->resolution) {
            if(e->base[j/e->resolution][i/e->resolution].water)
                fprintf(f,"%d%c ",e->base[j/e->resolution][i/e->resolution].height,'V');
            else
                fprintf(f,"%d ",e->base[j/e->resolution][i/e->resolution].height);
        }
        fprintf(f,"\n");
    }
    fclose(f);
}

/**
 * Domborzatot olvas be fájlból
 * @param fileName a forrás fájl elérési útvonala
 * @return a domborzatot leíró Environment struktúra
 * @author Márton
 */
Environment ReadEnvFromFile(const char *fileName) {
    FILE* f = fopen(fileName, "r");
    Environment env;
    fscanf(f, "%d", &env.w);
    fscanf(f, "%d", &env.h);
    fscanf(f, "%d",&env.resolution);
    env.base = (EnvironmentPoint**)Allocate2DArr(env.w/env.resolution+1, env.h/env.resolution+1, sizeof(EnvironmentPoint));
    char buf[10];
    for (int i = 0; i <= env.h; i+=env.resolution) {
        for (int j = 0; j <=env.w ; j+=env.resolution) {
            fscanf(f,"%s",buf);
            if(strchr(buf, 'V')){
                env.base[j/env.resolution][i/env.resolution].water = true;
            }
            else{
                env.base[j/env.resolution][i/env.resolution].water = false;
            }
            sscanf(buf,"%d",&env.base[j/env.resolution][i/env.resolution].height);
        }
    }

    fclose(f);
    return env;
}
