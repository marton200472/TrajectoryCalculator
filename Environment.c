#include "Environment.h"
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

int** GenerateRandomEnvironment(int w, int h) {
    srand(time(0));
    int **base = malloc((w+2)*sizeof(int*));
    for (int i = 0; i < w+2; ++i) {
        base[i] = malloc((h+2)*sizeof(int));
        for (int j = 0; j < h+2; ++j) {
            base[i][j] = rand()%255;
        }
    }
    int **env = malloc(w*sizeof(int*));
    for (int i = 0; i < w; ++i) {
        env[i] = malloc(h*sizeof(int));
        for (int j = 0; j < h; ++j) {
            int sum=0;
            for (int k = -1; k <= 1; ++k) {
                for (int l = -1; l <= 0; ++l) {
                    sum+=base[i+k+1][j+l+1];
                }
            }
            env[i][j] = sum / 8;
        }
    }
    for (int i = 0; i < w; ++i) {
        free(base[i]);
    }
    free(base);
    return env;
}
