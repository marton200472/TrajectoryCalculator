#include "Environment.h"
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "noise.h"
#include "2darr.h"

void avg(int **base, int **env, int** rm, int w, int h)
{
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            base[i][j] = env[i][j];
        }
    }

    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            if (rm[i/100][j/100] == 0){
                continue;
            }
            int sum=0,c=0;
            int xtrsh = rand()%20+1,ytresh=rand()%20+1;
            //int xtrsh = rm[i/100][j/100],ytresh=rm[i/100][j/100];
            for (int k = -xtrsh; k <= xtrsh; ++k) {
                for (int l = -ytresh; l <= ytresh; ++l) {
                    if (i+k >= 0 && i+k < w && j+l >= 0 && j+l < h)
                    {
                        c++;
                        sum+=base[i+k][j+l];
                    }
                }
            }
            env[i][j] = sum / c;
        }
    }
    for (int i = 0; i < w/100; ++i) {
        for (int j = 0; j < h/100; ++j) {
            rm[i][j] /= 2;
        }
    }
}

int** GenerateRandomEnvironment(int w, int h) {

    double** base = GeneratePerlinNoise(w,h,250);

    double max =    0;
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            if (base[i][j] > max)
                max = base[i][j];
            //base[i][j] *= 3;
        }
    }
    printf("%f\t",max);
    /*double weight = 1, ws = 2;
    for (int k = 512; k >= 8; k/=2) {
        double **curr = GeneratePerlinNoise(w,h,k);
        for (int i = 0; i < w; ++i) {
            for (int j = 0; j < h; ++j) {
                base[i][j] +=curr[i][j]*weight;
            }
        }
        Free2DArr(curr,w);
        ws+=weight;
        weight/=2;
    }


*/

    int** env = (int**)Allocate2DArr(w,h,sizeof(int));
    for (int i = 0; i < w; ++i) {
        for (int j = 0; j < h; ++j) {
            env[i][j] = pow(base[i][j],2)*1000;
        }
    }

    Free2DArr(base, w);
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
    return env;
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
