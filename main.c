#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "environment.h"
#include "debugmalloc.h"
#include "2darr.h"
#include "colortools.h"
#include "trajectory.h"
#include <pthread.h>
#include "file.h"

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
            EnvironmentPoint ep = GetHeightAtCoordinates(data->env, i, j);
            data->target[(data->env->w + 2 * data->xpad) * (j+data->ypad) + i+data->xpad]=height_to_rgb(ep.water?-1:ep.height);
        }
    }
}

SDL_Texture* RenderEnvironmentToTexture(SDL_Renderer *renderer, Environment *env, SDL_Rect *mapRect, int* xpadding, int* ypadding)
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
    *xpadding = xpad;
    *ypadding = ypad;
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

typedef struct DoubleInputField {
    char buffer[20];
    SDL_Rect bounds;
} DoubleInputField;

void RenderDoubleInputField(SDL_Renderer *renderer, DoubleInputField field)
{
    SDL_SetRenderDrawColor(renderer,30,30,30,255);
    SDL_RenderFillRect(renderer,&field.bounds);
}

typedef SDL_Point Point;

enum InputType {
    Input_None, Input_ArtyPos, Input_TargetPos
};

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    bool randomMap=false, saveMap=false;
    int rW=5000, rH=5000;
    int mapPathIndex = -1;

    for (int i = 0; i < argc; ++i) {
        printf("%s\n",argv[i]);
    }

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i],"--generate") == 0)
        {
            randomMap = true;
            if (argc < i+2)
            {
                printf("Generating default size (50km x 50km)\n");
            }
            else{
                sscanf(argv[++i],"%d", &rW);
                sscanf(argv[++i],"%d", &rH);
            }


        }
        else if(strcmp(argv[i], "--file") == 0)
        {
            if(randomMap)
                saveMap = true;
            mapPathIndex = ++i;
        }
    }

    if(mapPathIndex == -1 && !randomMap){
        randomMap = true;
    }



    SDL_Window* window = SDL_CreateWindow("Trajectory Calculator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1100, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    stringRGBA(renderer, 350, 300, "Loading...", 255, 255, 255, 255);
    SDL_RenderPresent(renderer);

    Environment env;
    if(randomMap)
    {
        srand(time(NULL));
        env = GenerateRandomEnvironment(rW,rH);
        if(saveMap)
            WriteEnvToFile(&env,argv[mapPathIndex]);
    }
    else
    {
        env = ReadEnvFromFile(argv[mapPathIndex]);
    }

    int xdim=env.w;
    int ydim=env.h;

    SDL_Rect mapRect = {0,0};
    int xpad, ypad;
    SDL_Texture* terrainTexture = RenderEnvironmentToTexture(renderer,&env, &mapRect, &xpad, &ypad);

    const SDL_Rect mapBoxRect = {301, 0, 800, 600};

    const SDL_Rect fullMap = mapRect;

    Point artyPos = {xdim/2, ydim/2};

    enum InputType inputType = Input_ArtyPos;

    bool mapClick = false;
    bool click = false;
    bool mouseMoved = false;
    int mouseDownX, mouseDownY;
    SDL_Rect mouseDownMapRect;

    DoubleInputField inputFields[2] = {
            {.bounds={.x=20, .y=20, .w=100, .h=40}},
            {.bounds={.x=20, .y=70, .w=100, .h=40}},
    };


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
                    if (ev.button.button == SDL_BUTTON_LEFT)
                    {
                        mouseDownX = ev.button.x;
                        mouseDownY = ev.button.y;
                        if (ev.button.x >= 300){
                            mouseDownMapRect = mapRect;
                            mapClick=true;
                        }
                        click = true;
                        mouseMoved = false;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (mapClick && ev.motion.x <= 300)
                        mapClick = false;

                    if (click){
                        mouseMoved = true;
                        if (mapClick)
                            MoveMap(&ev.motion,mouseDownX, mouseDownY,&fullMap,&mouseDownMapRect,&mapRect);
                    }

                    break;
                case SDL_MOUSEBUTTONUP:
                    if (ev.button.button == SDL_BUTTON_LEFT)
                    {


                        if (!mouseMoved)
                        {
                            if (mapClick)
                            {
                                Point np = (Point){.x = min(max(mapRect.x+mapRect.w/800.*(ev.button.x-300)-xpad,0),xdim-1), .y = min(max(mapRect.y+mapRect.h/600.*ev.button.y-ypad,0),ydim-1)};
                                if (!GetHeightAtCoordinates(&env,np.x, np.y).water)
                                artyPos = np;
                            }
                        }

                        click = false;
                        mapClick=false;
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

        for (int i = 0; i < 2; ++i) {
            RenderDoubleInputField(renderer, inputFields[i]);
        }

        if (artyPos.x+xpad >= mapRect.x && artyPos.x+xpad <= mapRect.x+mapRect.w &&
            artyPos.y+ypad >= mapRect.y && artyPos.y+ypad <= mapRect.y+mapRect.h  )
        {
            filledCircleColor(renderer,301+800./mapRect.w*(artyPos.x+xpad-mapRect.x), 600./mapRect.h*(artyPos.y+ypad-mapRect.y),5,0xFF0000FF);
        }

        char tavStr[10];
        int tav;
        if(mapRect.w > 5000)
        {
            strcpy(tavStr, "10 km");
            tav = 1000;
        }
        else if(mapRect.w > 2000)
        {
            strcpy(tavStr, "5 km");
            tav = 500;
        }
        else if(mapRect.w > 1500)
        {
            strcpy(tavStr, "5 km");
            tav = 500;
        }
        else if(mapRect.w > 500)
        {
            strcpy(tavStr, "1 km");
            tav = 100;
        }
        else
        {
            strcpy(tavStr, "250 m");
            tav = 25;
        }
        for (int i = 0; i < 3; ++i)
            lineRGBA(renderer,1100-25-(tav*(800./mapRect.w)),560+i, 1100-25,560+i,229, 52, 235,255);

        stringRGBA(renderer, (1100-25-(tav*(800./mapRect.w)) + 1100-25) / 2 - 10, 565,tavStr,229, 52, 235, 255);


        char artyPosText[50];
        sprintf(artyPosText,"Arty pos: %d %d %d",artyPos.x, artyPos.y, GetHeightAtCoordinates(&env,artyPos.x,artyPos.y));
        stringColor(renderer,20,30, artyPosText, 0xFFFFFFFF);

        SDL_RenderPresent(renderer);

        SDL_Delay(20);
    }

    SDL_Quit();

    FreeEnvironment(&env);
    return 0;
}
