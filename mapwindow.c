#include "mapwindow.h"
#include "environment.h"
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <pthread.h>



bool quitMapWindow = false;
SDL_Window *mapWindow;
SDL_Renderer *mapRenderer;
SDL_Texture *terrainTexture;

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




void InitMapWindow(MapWindowData *data) {
    mapWindow = SDL_CreateWindow("Trajectory Calculator - Map", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    mapRenderer = SDL_CreateRenderer(mapWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(mapRenderer, 0, 0, 0, 255);
    SDL_RenderClear(mapRenderer);
    stringRGBA(mapRenderer, 350, 300, "Loading...", 255, 255, 255, 255);
    SDL_RenderPresent(mapRenderer);

    int xdim = data->environment->w, ydim=data->environment->h;

    SDL_Surface* surface = SDL_CreateRGBSurface(0,xdim,ydim,32,
                                                0x00FF0000,
                                                0x0000FF00,
                                                0x000000FF,
                                                0xFF000000);
    uint32_t* pixels = surface->pixels;
    SDL_LockSurface(surface);
    for (int i = 0; i < xdim; ++i) {
        for (int j = 0; j < ydim; ++j) {
            pixels[xdim * j + i]=height_to_rgb(GetHeightAtCoordinates(data->environment, i, j));
        }
    }
    SDL_UnlockSurface(surface);

    terrainTexture = SDL_CreateTextureFromSurface(mapRenderer, surface);
}


void RenderMapWindow(MapWindowData *data) {

    //render stuff
    SDL_RenderCopy(mapRenderer, terrainTexture, NULL, NULL);
    SDL_RenderPresent(mapRenderer);
}

