#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include "environment.h"
#include "debugmalloc.h"
#include "2darr.h"
#include "colortools.h"
#include "trajectory.h"


#include "file.h"
#include "minmax.h"
#include "parameter.h"

#ifdef __unix__
#include <pthread.h>
#endif


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
    return NULL;
}

SDL_Texture* RenderEnvironmentToTexture(SDL_Renderer *renderer, Environment *env, SDL_Rect *mapRect, int* xpadding, int* ypadding)
{
    int xpad=0, ypad=0;
    if (env->w / (double)env->h > 4./3)
    {
        ypad = (int)(env->w*3./4 - env->h) / 2 + 1;
    }
    else if(env->w / (double)env->h < 4./3){
        xpad = (int)(env->h*4./3 - env->w) / 2 + 1;
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
#ifdef __unix__


    pthread_t threads[8];
    Section data[8];
    for (int i=0; i<8;++i){
        data[i] = (Section){.target=pixels, .env = env, .xpad = xpad, .ypad = ypad, .start = env->w/8*i, .end = i==7?env->w:env->w/8*(i+1)};
        pthread_create(&threads[i],NULL,RenderSection,&data[i]);
    }


    for (int i = 0; i < 8; ++i) {
        pthread_join(threads[i],NULL);
    }
#else
    for (int i = 0; i < env->w; ++i)
        for (int j = 0; j < env->h; ++j) {
            EnvironmentPoint ep = GetHeightAtCoordinates(env, i, j);
            pixels[(env->w + 2 * xpad) * (j+ypad) + i+xpad] = height_to_rgb(ep.water?-1:ep.height);
        }
#endif
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
        proposed.x += (int)(posx*proposed.w*(1.-multiplier));
        proposed.y += (int)(posy*proposed.h*(1.-multiplier));
    }
    else
    {
        multiplier = 1.25;
        proposed.x -= (int)(posx*(multiplier - 1)*proposed.w);
        proposed.y -= (int)(posy*(multiplier - 1)*proposed.h);
    }


    proposed.w = (int)(proposed.w * multiplier);
    proposed.h = (int)(proposed.h * multiplier);

    if (proposed.x + proposed.w >  max_size->w)
        proposed.x = max_size->w-proposed.w;
    if (proposed.y + proposed.h >  max_size->h)
        proposed.y = max_size->h-proposed.h;


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



void MoveMap(SDL_MouseMotionEvent *ev, int startX, int startY, const SDL_Rect *max_size,SDL_Rect *startMapRect, SDL_Rect *mapRect)
{
    double multiplier = mapRect->w/800.;

    int nx = (int)(startMapRect->x - (ev->x-startX)*multiplier);
    int ny = (int)(startMapRect->y - (ev->y-startY)*multiplier);

    mapRect->x = min(max(nx, 0), max_size->w-mapRect->w);
    mapRect->y = min(max(ny, 0), max_size->h-mapRect->h);
}




enum InputType {
    Input_None, Input_ArtyPos, Input_TargetPos
};

void RenderPositionData(SDL_Renderer *renderer, enum InputType inputType, Point artyPos, Point targetPos, Environment *env)
{
    char stringBuf[50];
    if(artyPos.x >= 0 && artyPos.y >= 0)
        sprintf(stringBuf, "Arty's position: %d %d %d", (int)artyPos.x, (int)artyPos.y, GetHeightAtCoordinates(env, (int)artyPos.x, (int)artyPos.y).height);
    else
        strcpy(stringBuf, "Arty's position: (unset)");
    if(inputType == Input_ArtyPos)
        stringRGBA(renderer, 20, 30, stringBuf, 255, 255, 0, 255);
    else
        stringRGBA(renderer, 20, 30, stringBuf, 255, 255, 255, 255);

    if(targetPos.x >= 0 && targetPos.y >= 0)
        sprintf(stringBuf, "Trgt's position: %d %d %d", (int)targetPos.x, (int)targetPos.y, GetHeightAtCoordinates(env, (int)targetPos.x, (int)targetPos.y).height);
    else
        strcpy(stringBuf, "Trgt's position: (unset)");
    if(inputType == Input_TargetPos)
        stringRGBA(renderer, 20, 50, stringBuf, 255, 255, 0, 255);
    else
        stringRGBA(renderer, 20, 50, stringBuf, 255, 255, 255, 255);
}

void RenderLengthReference(SDL_Renderer *renderer, SDL_Rect mapRect)
{
    char tavStr[10];
    int tav;
    if(mapRect.w > 5000)
    {
        strcpy(tavStr, "10 km");
        tav = 1000;
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
        lineRGBA(renderer,(short)(1100-25-(tav*(800./mapRect.w))),(short)(560+i), 1100-25,(short)(560+i),229, 52, 235,255);

    stringRGBA(renderer, (short)((1100-25-(tav*(800./mapRect.w)) + 1100-25) / 2 - 10), 565,tavStr,229, 52, 235, 255);
}

void HandleArrowKeys(SDL_KeyboardEvent ev, Point *artyPos, Point *targetPos, enum InputType inputType, Environment *env)
{
    Point *ptc;
    if(inputType == Input_ArtyPos)
        ptc = artyPos;
    else if(inputType == Input_TargetPos)
        ptc = targetPos;
    else
        return;

    switch (ev.keysym.scancode) {
        case SDL_SCANCODE_LEFT:
            if(ptc->x > 0 && !(inputType == Input_ArtyPos && GetHeightAtCoordinates(env,artyPos->x-1,artyPos->y).water))
                ptc->x--;
            break;
        case SDL_SCANCODE_RIGHT:
            if(ptc->x < env->w-1 && !(inputType == Input_ArtyPos && GetHeightAtCoordinates(env,artyPos->x+1,artyPos->y).water))
                ptc->x++;
            break;
        case SDL_SCANCODE_UP:
            if(ptc->y > 0 && !(inputType == Input_ArtyPos && GetHeightAtCoordinates(env,artyPos->x,artyPos->y-1).water))
                ptc->y--;
            break;
        case SDL_SCANCODE_DOWN:
            if(ptc->y < env->h-1 && !(inputType == Input_ArtyPos && GetHeightAtCoordinates(env,artyPos->y+1,artyPos->y).water))
                ptc->y++;
            break;
        default:
            break;
    }
}

typedef struct ResultDisplayField {
    AngleResult angleResult;
    char text[64];
    SDL_Rect bounds;
} ResultDisplayField;

typedef struct DoubleInputField {
    char name[32];
    double value;
    SDL_Rect bounds;
    char unit[4];
} DoubleInputField;

enum SimSpeed {
    SimSpeed_Instant=0,SimSpeed_Quick=1,SimSpeed_RealTime=2, SimSpeed_OptionCount=3
};

typedef struct SimSpeedInputField {
    int value;
    SDL_Rect bounds;
    const char *options[SimSpeed_OptionCount];
} SimSpeedInputField;




bool IsInsideBounds(SDL_Rect *bounds, int x, int y)
{
    return x >= bounds->x && x < bounds->x + bounds->w && y >= bounds->y && y < bounds->y + bounds->h;
}

void RenderDoubleInputField(SDL_Renderer *renderer, DoubleInputField *inputField)
{
    SDL_SetRenderDrawColor(renderer,50,50,50,255);
    SDL_RenderDrawRect(renderer,&inputField->bounds);
    char buf[100];
    sprintf(buf, "%s: %.2lf%s",inputField->name,inputField->value, inputField->unit);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w/2 - strlen(buf)/2*8), (short)(inputField->bounds.y+inputField->bounds.h/2-3), buf,255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + 5), (short)(inputField->bounds.y+inputField->bounds.h/2-3), "<",255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w - 13), (short)(inputField->bounds.y+inputField->bounds.h/2-3), ">",255,255,255,255);
}

void RenderSimSpeedInputField(SDL_Renderer *renderer, SimSpeedInputField *inputField)
{
    SDL_SetRenderDrawColor(renderer,50,50,50,255);
    SDL_RenderDrawRect(renderer,&inputField->bounds);
    char buf[100];
    sprintf(buf, "Simulation: %s",inputField->options[inputField->value]);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w/2 - strlen(buf)/2*8), (short)(inputField->bounds.y+inputField->bounds.h/2-3), buf,255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + 5), (short)(inputField->bounds.y+inputField->bounds.h/2-3), "<",255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w - 13), (short)(inputField->bounds.y+inputField->bounds.h/2-3), ">",255,255,255,255);
}

void RefreshCalculationResults(Point *artyPos, Point *targetPos,ArtilleryData *artyData,Environment *env, ResultDisplayField *resultFields, double *dX, int *dZ)
{
    if (artyPos->x >=0 && artyPos->y >= 0 && targetPos->x >= 0 && targetPos->y >= 0)
    {
        *dX = sqrt(pow(targetPos->x - artyPos->x, 2) + pow(targetPos->y - artyPos->y,2)) * 10;
        //subtract 1 because gun is at 1 m height
        *dZ = (int)GetHeightAtCoordinates(env,targetPos->x, targetPos->y).height - (int)GetHeightAtCoordinates(env, artyPos->x, artyPos->y).height-1;




        for (int i = 0; i < artyData->optionCount; ++i) {

            AngleResult ar = FindVerticalAngleToTarget(artyData->options[i],*dX,*dZ, artyData);
            resultFields[i].angleResult = ar;
            if (ar.resultCount > 0)
            {
                int cw = sprintf(resultFields[i].text,"%d. (%.2lf m/s): %.1lf", i+1, artyData->options[i], ar.results[0]);
                if(ar.resultCount == 2)
                    sprintf(resultFields[i].text + cw," %.1lf", ar.results[1]);
            }

            else
                sprintf(resultFields[i].text,"%d. (%.2lf m/s): Out of range",i+1,artyData->options[i]);
            resultFields[i].bounds = (SDL_Rect){20, 300+30*i, 240, 28};


        }


    }
}

void RenderLoadingScreen(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    stringRGBA(renderer, 500, 300, "Loading...", 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
}

double time_secs()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}

void StartSimulation(double *startTime, enum SimSpeed simSpeed, bool *started, bool *ended)
{
    if (simSpeed == SimSpeed_Instant)
    {
        *ended = true;
        *started = true;
    }
    else{
        *startTime = time_secs();
        *started = true;
        *ended = false;
    }
}

void ProcessSimulation(SDL_Renderer *renderer, TrajectoryInfo  *trInfo, SDL_Rect mapRect, int xPad, int yPad,double pDiff,double startTime, enum SimSpeed simSpeed, bool started, bool *ended, Point artyPos, double azimuth, double v0, double angle)
{
    if (!started)
        return;



    int iters;
    double t = time_secs() - startTime;
    if (simSpeed == SimSpeed_Quick)
        iters = (int)(t * 10) / SIMULATION_TIMESTEP_S;
    else
        iters = (int)t / SIMULATION_TIMESTEP_S;

    if (iters >= trInfo->pointCount)
        *ended = true;


    if (*ended)
        iters = trInfo->pointCount;

    for (int i = 0; i < iters; ++i) {
        if (trInfo->points[i].position.x + xPad >= mapRect.x && trInfo->points[i].position.x + xPad <= mapRect.x + mapRect.w &&
                trInfo->points[i].position.y + yPad >= mapRect.y && trInfo->points[i].position.y + yPad <= mapRect.y + mapRect.h  ) {

            Point pos = {(int) (301 + 800. / mapRect.w * (trInfo->points[i].position.x + xPad - mapRect.x) + pDiff),
                         (int) (600. / mapRect.h * (trInfo->points[i].position.y + yPad - mapRect.y) + pDiff)};
            filledCircleRGBA(renderer, (short)pos.x, (short)pos.y, 2, trInfo->points[i].color, trInfo->points[i].color,
                             trInfo->points[i].color, 255);
        }
    }

    if (!*ended)
    {
        Point pos = GetProjectileMapLocationAtTime(artyPos, azimuth, simSpeed == SimSpeed_Quick ? t * 10 : t, v0, angle);
        if (pos.x + xPad >= mapRect.x && pos.x + xPad <= mapRect.x + mapRect.w &&
            pos.y + yPad >= mapRect.y && pos.y + yPad <= mapRect.y + mapRect.h)
        {
            Point posOnScreen = {(int) (301 + 800. / mapRect.w * (pos.x + xPad - mapRect.x) + pDiff),
                                 (int) (600. / mapRect.h * (pos.y + yPad - mapRect.y) + pDiff)};
            filledCircleRGBA(renderer,(short)posOnScreen.x,(short)posOnScreen.y,4,255,0,0,255);
        }

    }


}

void UpdateTrajectoryInfoIfNeeded(Point artyPos, Environment *env, TrajectoryInfo *trInfo, double azimuth, double launchSpeed, double verticalAngle)
{
    if (artyPos.x != -1 && artyPos.y != -1)
    {
        free(trInfo->points);
        *trInfo = CalculateTrajectory(env, &artyPos, azimuth, launchSpeed, verticalAngle);
    }
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL));
    bool randomMap=false, saveMap=false;
    int rW=5000, rH=5000;
    int mapPathIndex = -1, artyDataFileIndex=-1;

    ProcessCommandLineArgs(argc, argv, &randomMap, &saveMap, &rW, &rH, &mapPathIndex, &artyDataFileIndex);

    ArtilleryData artyData = ReadArtilleryData(argv[artyDataFileIndex]);

    SDL_Window* window = SDL_CreateWindow("Trajectory Calculator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1100, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    RenderLoadingScreen(renderer);

    Environment env = LoadEnvironment(randomMap, saveMap,mapPathIndex,rW,rH,argv);

    SDL_Rect mapRect = {0,0};

    int xPad, yPad;
    SDL_Texture* terrainTexture = RenderEnvironmentToTexture(renderer, &env, &mapRect, &xPad, &yPad);

    const SDL_Rect mapBoxRect = {301, 0, 800, 600};
    const SDL_Rect fullMap = mapRect;

    Point artyPos = {-1, -1},
          targetPos = {-1,-1};



    bool mapClick = false;
    bool click = false;
    bool mapRightCLicked = false;
    int mouseDownX, mouseDownY;
    SDL_Rect mouseDownMapRect;
    enum InputType inputType = Input_ArtyPos;

    ResultDisplayField *resultFields = malloc(artyData.optionCount*sizeof(ResultDisplayField));
    int selectedResult = -1, selectedAngle = -1;

    //x and z difference between gun and target
    double deltaX;
    int deltaZ;

    DoubleInputField azimuth = {.bounds = {20,120,240,25}, .value = 0, .name = "Azimuth", .unit = "deg"};
    DoubleInputField verticalAngle = {.bounds = {20,150,240,25}, .value = 0, .name = "Angle", .unit = "deg"};
    DoubleInputField launchSpeed = {.bounds = {20, 180, 240, 25}, .value=artyData.options[0],.name = "Launch speed", .unit = "m/s"};
    SimSpeedInputField simSpeed = {.bounds = {20,210,240,25}, .value = SimSpeed_Instant, .options = {"Instant","Quick","Real-time"}};

    SDL_Rect shootButtonBounds = {90,270,100,25};

    bool simulationStarted=false;
    bool simulationFinished = false;
    double simulationStartTime;
    TrajectoryInfo trInfo = {.pointOfImpact = {-1,-1}};


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
                    if (ev.key.type == SDL_KEYDOWN)
                    switch (ev.key.keysym.scancode) {
                        case SDL_SCANCODE_TAB:
                            switch (inputType) {
                                case Input_ArtyPos:
                                    inputType = Input_TargetPos;
                                    break;
                                case Input_TargetPos:
                                    inputType = Input_None;
                                    break;
                                case Input_None:
                                    inputType = Input_ArtyPos;
                            }
                            break;
                            case SDL_SCANCODE_LEFT: case SDL_SCANCODE_RIGHT: case SDL_SCANCODE_UP: case SDL_SCANCODE_DOWN:
                                HandleArrowKeys(ev.key,&artyPos, &targetPos,inputType,&env);
                                RefreshCalculationResults(&artyPos, &targetPos, &artyData, &env, resultFields, &deltaX, &deltaZ);
                                UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                                StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                                break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    ZoomMap(&ev.wheel,&fullMap, &mapRect);
                    int x, y;
                    SDL_GetMouseState(&x,&y);
                    if (IsInsideBounds(&azimuth.bounds, x,y))
                    {
                        azimuth.value = round((azimuth.value + ev.wheel.y*2.));
                        while (azimuth.value >= 360.)
                            azimuth.value -= 360.;
                        while (azimuth.value < 0)
                            azimuth.value+=360.;
                        StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                    }
                    else if (IsInsideBounds(&verticalAngle.bounds, x,y))
                    {
                        verticalAngle.value = round((verticalAngle.value + ev.wheel.y*2.));
                        if (verticalAngle.value < artyData.minAngle)
                            verticalAngle.value = artyData.minAngle;
                        else if(verticalAngle.value > artyData.maxAngle)
                            verticalAngle.value = artyData.maxAngle;
                        StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                    }
                    else if (IsInsideBounds(&launchSpeed.bounds,x,y))
                    {
                        int currentIndex = 0;
                        for (int i = 0; i < artyData.optionCount; ++i) {
                            if (artyData.options[i] == launchSpeed.value)
                            {
                                currentIndex = i;
                                break;
                            }
                        }
                        currentIndex+=ev.wheel.y;
                        if (currentIndex >= artyData.optionCount)
                            currentIndex=0;
                        else if (currentIndex < 0)
                            currentIndex = artyData.optionCount-1;
                        launchSpeed.value = artyData.options[currentIndex];
                        StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                    }
                    else if(IsInsideBounds(&simSpeed.bounds, x,y))
                    {
                        simSpeed.value+=ev.wheel.y;
                        if (simSpeed.value < 0)
                            simSpeed.value = SimSpeed_OptionCount-1;
                        else if(simSpeed.value >= SimSpeed_OptionCount)
                            simSpeed.value = 0;
                        StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                    }
                    UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
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
                        for (int i = 0; i < artyData.optionCount; ++i) {
                            if (IsInsideBounds(&resultFields[i].bounds, ev.button.x, ev.button.y) && resultFields[i].angleResult.resultCount > 0)
                            {
                                if (selectedResult != i)
                                {
                                    selectedResult = i;
                                    selectedAngle = 0;
                                }
                                else
                                {
                                    selectedAngle++;
                                    if (selectedAngle == resultFields[i].angleResult.resultCount)
                                        selectedAngle = 0;
                                }
                                verticalAngle.value = resultFields[i].angleResult.results[selectedAngle];
                                launchSpeed.value = artyData.options[i];

                                double az = 360. - RadToDeg(atan2(artyPos.y - targetPos.y, targetPos.x - artyPos.x) - M_PI_2);
                                while (az >= 360.)
                                    az -= 360.;
                                while (az < 0.)
                                    az += 360.;
                                azimuth.value = az;
                                StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                            }
                        }

                        if (IsInsideBounds(&azimuth.bounds, ev.button.x,ev.button.y))
                        {
                            int dir = ev.button.x < azimuth.bounds.x+azimuth.bounds.w/2 ? -1 : 1;
                            azimuth.value = round((azimuth.value + dir*0.5)*10.)/10.;
                            while (azimuth.value >= 360.)
                                azimuth.value -= 360.;
                            while (azimuth.value < 0)
                                azimuth.value+=360.;
                            StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                        }
                        else if (IsInsideBounds(&verticalAngle.bounds, ev.button.x,ev.button.y))
                        {
                            int dir = ev.button.x < verticalAngle.bounds.x+verticalAngle.bounds.w/2 ? -1 : 1;
                            verticalAngle.value = round((verticalAngle.value + dir*0.5)*10.)/10.;
                            if (verticalAngle.value < artyData.minAngle)
                                verticalAngle.value = artyData.minAngle;
                            else if(verticalAngle.value > artyData.maxAngle)
                                verticalAngle.value = artyData.maxAngle;
                            StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                        }
                        else if (IsInsideBounds(&launchSpeed.bounds,ev.button.x, ev.button.y))
                        {
                            int dir = ev.button.x < launchSpeed.bounds.x+launchSpeed.bounds.w/2 ? -1 : 1;
                            int currentIndex = 0;
                            for (int i = 0; i < artyData.optionCount; ++i) {
                                if (artyData.options[i] == launchSpeed.value)
                                {
                                    currentIndex = i;
                                    break;
                                }
                            }
                            currentIndex+=dir;
                            if (currentIndex < 0)
                                currentIndex = artyData.optionCount-1;
                            currentIndex %= artyData.optionCount;
                            launchSpeed.value = artyData.options[currentIndex];
                            StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                        }
                        else if(IsInsideBounds(&simSpeed.bounds, ev.button.x, ev.button.y))
                        {
                            int dir = ev.button.x < launchSpeed.bounds.x+launchSpeed.bounds.w/2 ? -1 : 1;
                            simSpeed.value+=dir;
                            if (simSpeed.value < 0)
                                simSpeed.value = SimSpeed_OptionCount-1;
                            else if(simSpeed.value >= SimSpeed_OptionCount)
                                simSpeed.value = 0;
                            StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                        }
                        else if(IsInsideBounds(&shootButtonBounds,ev.button.x,ev.button.y))
                        {
                            StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                        }
                        UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                    }
                    else if (ev.button.button == SDL_BUTTON_RIGHT)
                    {
                        mapRightCLicked = ev.button.x > 300;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (mapClick && ev.motion.x <= 300)
                        mapClick = false;

                    if (click){
                        if (mapClick)
                            MoveMap(&ev.motion,mouseDownX, mouseDownY,&fullMap,&mouseDownMapRect,&mapRect);
                    }

                    break;
                case SDL_MOUSEBUTTONUP:
                    if (ev.button.button == SDL_BUTTON_LEFT)
                    {



                        click = false;
                        mapClick=false;
                    }
                    else if(ev.button.button == SDL_BUTTON_RIGHT)
                    {
                        if (ev.button.x > 300 && mapRightCLicked)
                        {
                            if (inputType == Input_ArtyPos)
                            {
                                Point np = (Point){.x = min(max((int)(mapRect.x +mapRect.w/800.*(ev.button.x-301) - xPad), 0), env.w - 1), .y = min(max((int)(mapRect.y + mapRect.h / 600. * ev.button.y - yPad), 0), env.h - 1)};
                                if (!GetHeightAtCoordinates(&env,np.x, np.y).water)
                                    artyPos = np;
                                StartSimulation(&simulationStartTime,simSpeed.value,&simulationStarted, &simulationFinished);
                            }
                            else if (inputType == Input_TargetPos)
                            {
                                targetPos = (Point){.x = min(max((int)(mapRect.x +mapRect.w/800.*(ev.button.x-301) - xPad), 0), env.w - 1), .y = min(max((int)(mapRect.y + mapRect.h / 600. * ev.button.y - yPad), 0), env.h - 1)};
                            }

                            RefreshCalculationResults(&artyPos, &targetPos, &artyData, &env, resultFields, &deltaX, &deltaZ);
                            UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                            mapRightCLicked = false;
                        }
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



        int pDiff = (int)(300./mapRect.w);

        ProcessSimulation(renderer,&trInfo,mapRect,xPad,yPad,pDiff,simulationStartTime,simSpeed.value,simulationStarted,&simulationFinished,artyPos,DegToRad(azimuth.value),launchSpeed.value,DegToRad(verticalAngle.value));


        if (artyPos.x + xPad >= mapRect.x && artyPos.x + xPad <= mapRect.x + mapRect.w &&
            artyPos.y + yPad >= mapRect.y && artyPos.y + yPad <= mapRect.y + mapRect.h  )
        {
            Point pos = {(int)(301 + 800. / mapRect.w * (artyPos.x + xPad - mapRect.x)), (int)(600. / mapRect.h * (artyPos.y + yPad - mapRect.y))};

            boxRGBA(renderer, (short)(pos.x - 5 + pDiff), (short)(pos.y - 5 + pDiff), (short)(pos.x + 5 + pDiff), (short)(pos.y + 5 + pDiff), 250, 165, 0, 255);
            rectangleRGBA(renderer, (short)(pos.x - 5 + pDiff), (short)(pos.y - 5 + pDiff), (short)(pos.x + 5 + pDiff+1), (short)(pos.y + 5 +pDiff+1), 0, 0, 0, 255);
        }

        if (targetPos.x + xPad >= mapRect.x && targetPos.x + xPad <= mapRect.x + mapRect.w &&
            targetPos.y + yPad >= mapRect.y && targetPos.y + yPad <= mapRect.y + mapRect.h  )
        {
            Point pos = {(int)(301 +800./mapRect.w*(targetPos.x + xPad - mapRect.x) + pDiff), (int)(600. / mapRect.h * (targetPos.y + yPad - mapRect.y) + pDiff)};
            filledCircleRGBA(renderer,(short)pos.x, (short)pos.y,5,255,0,0,255);
            circleRGBA(renderer, (short)pos.x, (short)pos.y, 5, 0,0,0,255);
        }

        if (simulationStarted && trInfo.pointOfImpact.x + xPad >= mapRect.x && trInfo.pointOfImpact.x + xPad <= mapRect.x + mapRect.w &&
            trInfo.pointOfImpact.y + yPad >= mapRect.y && trInfo.pointOfImpact.y + yPad <= mapRect.y + mapRect.h)
        {
            Point pos = {(int) (301 + 800. / mapRect.w * (trInfo.pointOfImpact.x + xPad - mapRect.x) + pDiff),
                         (int) (600. / mapRect.h * (trInfo.pointOfImpact.y + yPad - mapRect.y) + pDiff)};
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
            SDL_RenderDrawLine(renderer,pos.x-5,pos.y-5,pos.x+5,pos.y+5);
            SDL_RenderDrawLine(renderer,pos.x-5,pos.y+5,pos.x+5,pos.y-5);
        }

        char buf[100];
        if (simulationStarted)
        {
            sprintf(buf, "Shell splash @ %d %d", (int)trInfo.pointOfImpact.x, (int)trInfo.pointOfImpact.y);
            if(trInfo.pointOfImpact.x < 0 || trInfo.pointOfImpact.x >= env.w || trInfo.pointOfImpact.y < 0 || trInfo.pointOfImpact.y >= env.h) {
                sprintf(buf,"Shell went out of map bounds");
                stringRGBA(renderer, 50, 248, buf, 255, 0, 0, 255);
            }
            else if (targetPos.x == -1 || targetPos.y == -1)
                stringRGBA(renderer,50,248,buf,255,255,255,255);
            else if (targetPos.x == trInfo.pointOfImpact.x && targetPos.y == trInfo.pointOfImpact.y)
                stringRGBA(renderer,50,248,buf,0,255,0,255);
            else
                stringRGBA(renderer,50,248,buf,255,0,0,255);
        }


        RenderLengthReference(renderer,mapRect);
        RenderPositionData(renderer,inputType,artyPos,targetPos, &env);

        RenderDoubleInputField(renderer, &azimuth);
        RenderDoubleInputField(renderer, &verticalAngle);
        RenderDoubleInputField(renderer, &launchSpeed);
        RenderSimSpeedInputField(renderer,&simSpeed);

        SDL_SetRenderDrawColor(renderer,50,50,50,255);
        SDL_RenderDrawRect(renderer,&shootButtonBounds);
        stringRGBA(renderer,(short)(shootButtonBounds.x+shootButtonBounds.w/2 - 24), (short)(shootButtonBounds.y+8),"Shoot!",255,255,255,255);


        if (artyPos.x >=0 && artyPos.y >= 0 && targetPos.x >= 0 && targetPos.y >= 0)
        {
            sprintf(buf, "dX: %.1lf m", deltaX);
            stringRGBA(renderer,20,80,buf,255,255,255,255);
            sprintf(buf, "dZ: %d m", deltaZ);
            stringRGBA(renderer,20,100,buf,255,255,255,255);


            for (int i = 0; i <artyData.optionCount; ++i) {
                SDL_SetRenderDrawColor(renderer,50,50,50,255);
                SDL_RenderDrawRect(renderer,&resultFields[i].bounds);
                stringRGBA(renderer,(short)(resultFields[i].bounds.x + 5),(short)(resultFields[i].bounds.y+10),resultFields[i].text,255,255,255,255);
            }
        }



        SDL_RenderPresent(renderer);
        SDL_Delay(20);
    }

    SDL_Quit();

    FreeEnvironment(&env);
    FreeArtilleryData(&artyData);
    free(trInfo.points);
    free(resultFields);
    return 0;
}
