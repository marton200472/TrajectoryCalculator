#include "Environment.h"
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "noise.h"
#include "2darr.h"



//TODO multithreading
int** GenerateRandomEnvironment(int w, int h) {

    Environment e;
    e.lbase = GenerateNoiseBase(w,h,500);
    e.sbase = GenerateNoiseBase(w,h,200);
    e.w = w;
    e.h = h;
    e.lres = 500;
    e.sres = 200;
    e.lw = 1.5;
    e.sw = 0.5;




    int** env = (int**)Allocate2DArr(w,h,sizeof(int));
    double min = 1;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            double kek = pow((GetPointValue(e.lbase,i,j,500)*1.5 + GetPointValue(e.sbase,i,j,200)*0.5)/2,7);
            env[i][j] = kek*2060-60;
            if(kek < min)
                min = kek;
        }
    }
    printf("%lf\n",min);
    return env;

    /*int bw=w,bh=h;

    srand(time(0));
    int **base = malloc(bw*sizeof(int*));
    for (int i = 0; i < bw; ++i) {
        base[i] = malloc(bh*sizeof(int));
        for (int j = 0; j < bh; ++j) {
            base[i][j] = rand()%255;
        }
    }
    int** rm = malloc((bw/100)*sizeof(int*));
    for (int i = 0; i < bw/100; ++i) {
        rm[i] = malloc((bh/100)*sizeof(int));
        for (int j = 0; j < bh/100; ++j) {
            rm[i][j] = rand()%12+5;
        }
    }
    int **env = malloc(bw*sizeof(int*));
    for (int i = 0; i < bw; ++i) {
        env[i] = malloc(bh*sizeof(int));
        for (int j = 0; j < bh; ++j) {
            env[i][j] = base[i][j];
        }
    }

    for (int i = 0; i < 3; ++i) {
        avg(base,env,rm,bw,bh);
    }

    for (int i = 0; i < bw; ++i) {
        free(base[i]);
    }
    free(base);

    /*for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            env[i][j] *= 2;
        }

    }*/
    /*int** fenv = malloc(w*sizeof(int*));

    for (int i = 0; i < w; ++i) {
        fenv[i] = malloc(h*sizeof(int));
        for (int j = 0; j < h; ++j) {
            int modi = i%100, modj = j%100;
            double pos1 = 1/sqrt(modi*modi+modj*modj);
            if (isinf(pos1)){
                fenv[i][j] = env[i/100][j/100];
                break;
            }

            double pos2 = 1/sqrt((100-modi)*(100-modi)+(100-modj)*(100-modj));
            if (isinf(pos2))
            {
                fenv[i][j] = env[i/100+1][j/100+1];
                break;
            }

            double pos3 = 1/sqrt(modi*modi+(100-modj)*(100-modj));
            if (isinf(pos3))
            {
                fenv[i][j] = env[i/100][j/100+1];
                break;
            }

            double pos4 = 1/sqrt((100-modi)*(100-modi)+modj*modj);
            if (isinf(pos4))
            {
                fenv[i][j] = env[i/100+1][j/100];
                break;
            }


            fenv[i][j] = (pos1*env[i/100][j/100] + pos2*env[i/100+1][j/100]+1 + pos3*env[i/100][j/100+1] + pos4*env[i/100+1][j/100]) / (2*(pos1+pos2+pos3+pos4));
        }
    }

    for (int i = 0; i < bw; ++i) {
        free(env[i]);
    }
    free(env);

    return fenv;*/
}
