#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include "Environment.h"
#include "debugmalloc.h"
#include <math.h>
#include <time.h>
#include "noise.h"
#include "2darr.h"
#include "trajectory.h"
#include "file.h"

SDL_Window *window;
SDL_Renderer *renderer;

uint32_t height_to_rgb(int height)
{
    if (height < 0)
        return 0xff15b2ff;
    if (height < 100)
        return 0xff86c979;
    if (height < 150)
        return 0xffb1d166;
    if (height < 200)
        return 0xffd9e18e;
    if (height < 300)
        return 0xffd9e18e;
    if (height < 500)
        return 0xfff9cd8c;
    if (height < 700)
        return 0xfff9ae75;
    if (height < 1000)
        return 0xffd19c70;
    if (height < 2000)
        return 0xffb28f71;
}

int main(int argc, char *argv[]) {
    debugmalloc_log_file("./debugmalloc.log");
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(0));

    window = SDL_CreateWindow("Trajectory Calculator",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    //renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);


    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    //SDL_Texture* texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STREAMING, 800, 600);

    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);
    stringRGBA(renderer,400,250,"Betöltés...", 255,255,255,255);
    SDL_RenderPresent(renderer);
    SDL_Delay(50);

    int xdim = 8000, ydim=6000;
    SDL_Surface* surface = SDL_CreateRGBSurface(0,xdim,ydim,32,
                                                0x00FF0000,
                                                0x0000FF00,
                                                0x000000FF,
                                                0xFF000000);
    int **env=GenerateRandomEnvironment(xdim,ydim);
    uint32_t* text = surface->pixels;
    //SDL_LockTexture(texture,NULL,&text,&pitch);
    for (int i = 0; i < xdim; ++i) {
        for (int j = 0; j < ydim; ++j) {
            //int xc = (int)((double)i/800*xdim), yc = (int)((double)j/600*ydim);
            //uint32_t color = height_to_rgb(env[xc][yc]);
            //SDL_SetRenderDrawColor(renderer,(color & 0xFF0000) >> 16,(color & 0xFF00) >> 8 ,color & 0xFF,255);
            //SDL_RenderDrawPoint(renderer,i,j);

            text[xdim*j+i]=height_to_rgb(env[i][j]);
        }
    }
    Free2DArr(env,xdim);

    //SDL_UnlockTexture(texture);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);

    SDL_RenderCopy(renderer,texture,NULL,NULL);

    SDL_RenderPresent(renderer);
    /*char fname[50];
    scanf("%s",fname);
    DynamicArray arr = ReadArtilleryV0Data(fname);
    for (int i = 0; i<arr.count; ++i){
        printf("Calculating data for 28500;20 with v0=%.2f: ", arr.data[i]);
        AngleResult ar = FindVerticalAngleToTarget(arr.data[i], 28500, 20);
        if (!ar.success)
            printf("Out of range\n");
        else
            printf("Results: %.2f° and %.2f°\n",ar.result1, ar.result2);
    }*/



    bool quit = false;
    SDL_Event ev;
    //main event loop
    while (!quit)
    {
        while (SDL_WaitEvent(&ev) && !quit)
        {
            switch (ev.type)
            {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
            SDL_RenderCopy(renderer,texture,NULL,NULL);
            circleRGBA(renderer,400,300,3000./xdim*400,255,0,0,255);
            SDL_RenderPresent(renderer);
        }

    }


    SDL_Quit();
    return 0;
}
