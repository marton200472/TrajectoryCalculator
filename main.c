#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "environment.h"
#include "debugmalloc.h"
#include "2darr.h"
#include "colortools.h"
#include <pthread.h>

typedef struct Section {
    unsigned *target;
    int start, end;
    int xpad, ypad;
    Environment *env;
} Section;

void* RenderSection(void *vargp)
{
    Section *data = vargp;
    for (int i = data->start; i < data->end; ++i){
        for (int j = 0; j < data->env->h; ++j) {
            data->target[(data->env->w + 2 * data->xpad) * (j+data->ypad) + i+data->xpad]=height_to_rgb(GetHeightAtCoordinates(data->env, i, j));
        }
    }
}

SDL_Texture* RenderEnvironmentToTexture(SDL_Renderer *renderer, Environment *env, SDL_Rect *mapRect)
{
    int xpad=0, ypad=0;
    if (env->w / (double)env->h > 4./3)
    {
        ypad = (env->w*3./4 - env->h) / 2 + 1;
    }
    else if(env->w / (double)env->h < 4./3){
        xpad = (env->h*4./3 - env->w) / 2 + 1;
    }

    mapRect->w = env->w+2*xpad;
    mapRect->h = env->h+2*ypad;

    SDL_Surface* surface = SDL_CreateRGBSurface(0,env->w + 2*xpad,env->h + 2*ypad,32,
                                                0x00FF0000,
                                                0x0000FF00,
                                                0x000000FF,
                                                0x00000000);
    SDL_LockSurface(surface);
    unsigned *pixels = surface->pixels;
    pthread_t threads[8];
    Section data[8];
    for (int i=0; i<8;++i){
        data[i] = (Section){.target=pixels, .env = env, .xpad = xpad, .ypad = ypad, .start = env->w/8*i, .end = i==7?env->w:env->w/8*(i+1)};
        pthread_create(&threads[i],NULL,RenderSection,&data[i]);
    }


    for (int i = 0; i < 8; ++i) {
        pthread_join(threads[i],NULL);
    }


    SDL_UnlockSurface(surface);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer,surface);
    SDL_FreeSurface(surface);
    return  texture;
}

void ZoomMap(SDL_MouseWheelEvent *ev,const SDL_Rect *max_size, SDL_Rect* mapRect)
{
    int x, y;
    SDL_GetMouseState(&x,&y);
    if (x<300)
        return;

    double posx = (x-300)/800., posy = y/600.;
    double multiplier;

    SDL_Rect proposed = *mapRect;

    if (ev->y>0){
        multiplier = 0.8;
        proposed.x += posx*proposed.w*(1.-multiplier);
        proposed.y += posy*proposed.h*(1.-multiplier);
    }
    else
    {
        multiplier = 1.25;
        proposed.x -= posx*(multiplier - 1)*proposed.w;
        proposed.y -= posy*(multiplier - 1)*proposed.h;
    }


    proposed.w *= multiplier;
    proposed.h *= multiplier;


    if (proposed.h > max_size->h || proposed.w > max_size->w)
    {
        *mapRect = *max_size;
        return;
    }

    if (proposed.h < 60 || proposed.w < 80)
        return;

    if (proposed.x < 0)
        proposed.x = 0;
    if (proposed.y < 0)
        proposed.y = 0;

    *mapRect = proposed;
}

int max(int a, int b)
{
    return a < b ? b : a;
}
int min(int a, int b)
{
    return a > b ? b : a;
}

void MoveMap(SDL_MouseMotionEvent *ev, int startX, int startY, const SDL_Rect *max_size,SDL_Rect *startMapRect, SDL_Rect *mapRect)
{
    double multiplier = mapRect->w/800.;

    int nx = startMapRect->x - (ev->x-startX)*multiplier;
    int ny = startMapRect->y - (ev->y-startY)*multiplier;

    mapRect->x = min(max(nx, 0), max_size->w-mapRect->w);
    mapRect->y = min(max(ny, 0), max_size->h-mapRect->h);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);


    SDL_Window* window = SDL_CreateWindow("Trajectory Calculator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1100, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    stringRGBA(renderer, 350, 300, "Loading...", 255, 255, 255, 255);
    SDL_RenderPresent(renderer);



    int xdim = 10000, ydim=10000;
    Environment env = GenerateRandomEnvironment(xdim,ydim);

    SDL_Rect mapRect = {0,0};
    SDL_Texture* terrainTexture = RenderEnvironmentToTexture(renderer,&env, &mapRect);

    const SDL_Rect mapBoxRect = {301, 0, 800, 600};

    const SDL_Rect fullMap = mapRect;

    bool moveMap = false;
    int mouseDownX, mouseDownY;
    SDL_Rect mouseDownMapRect;


    bool quit = false;
    SDL_Event ev;
    while (!quit)
    {
        while (SDL_PollEvent(&ev)){
            switch (ev.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_MOUSEWHEEL:
                    ZoomMap(&ev.wheel,&fullMap, &mapRect);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (ev.button.button == SDL_BUTTON_LEFT && ev.button.x >= 300)
                    {
                        mouseDownX = ev.button.x;
                        mouseDownY = ev.button.y;
                        mouseDownMapRect = mapRect;
                        moveMap=true;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (moveMap && ev.motion.x <= 300)
                        moveMap = false;

                    if (moveMap){
                        MoveMap(&ev.motion,mouseDownX, mouseDownY,&fullMap,&mouseDownMapRect,&mapRect);
                    }

                    break;
                case SDL_MOUSEBUTTONUP:
                    if (moveMap && ev.button.button == SDL_BUTTON_LEFT)
                    {
                        moveMap=false;
                    }
                    break;

            }
        }




        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer,terrainTexture,&mapRect,&mapBoxRect);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 300,0, 300,600);
        SDL_RenderDrawLine(renderer, 299,0, 299,600);

        SDL_RenderPresent(renderer);

        SDL_Delay(20);
    }

    SDL_Quit();

    FreeEnvironment(&env);
    return 0;
}
