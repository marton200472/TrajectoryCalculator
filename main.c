#include <stdio.h>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <time.h>
#include "environment.h"
#include "debugmalloc.h"
#include "2darr.h"
#include "trajectory.h"
#include "file.h"
#include "parameter.h"
#include "simulation.h"
#include "controlpanel.h"
#include "render.h"
#include "map.h"
#include "bounds.h"


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL)+clock());
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
    bool mapRightClicked = false;
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
    SDL_Rect controlPanelBounds = {0,0,300,600};

    SimulationState simState = {false,false,0};
    TrajectoryInfo trInfo = {.pointOfImpact = {-1,-1}};


    bool quit = false;
    SDL_Event ev;
    int x, y;
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
                                if(inputType != Input_None){
                                    HandleArrowKeys(ev.key,&artyPos, &targetPos,inputType,&env);
                                    RefreshCalculationResults(&artyPos, &targetPos, &artyData, &env, resultFields, &deltaX, &deltaZ);
                                    UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                                    StartSimulation(simSpeed.value,&simState);
                                }
                                break;
                        default:
                            break;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    SDL_GetMouseState(&x,&y);
                    if (!IsInsideBounds(&controlPanelBounds,x,y))
                        ZoomMap(&ev.wheel,&fullMap, &mapRect, x, y);
                    else if(HandleScroll(&azimuth,&verticalAngle,&launchSpeed,&simSpeed,artyData, ev.wheel, x,y))
                    {
                        StartSimulation(simSpeed.value,&simState);
                        UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (ev.button.button == SDL_BUTTON_LEFT)
                    {
                        mouseDownX = ev.button.x;
                        mouseDownY = ev.button.y;
                        if (!IsInsideBounds(&controlPanelBounds,ev.button.x,ev.button.y)){
                            mouseDownMapRect = mapRect;
                            mapClick=true;
                        }
                        else if(HandleLeftClick(&azimuth,&verticalAngle,&launchSpeed,&simSpeed,&shootButtonBounds,resultFields,artyPos,targetPos,&selectedResult,&selectedAngle,artyData,ev.button.x, ev.button.y))
                        {
                            StartSimulation(simSpeed.value,&simState);
                            UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);
                        }

                    }
                    else if (ev.button.button == SDL_BUTTON_RIGHT)
                    {
                        mapRightClicked = ev.button.x > 300;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (mapClick && ev.motion.x <= 300)
                        mapClick = false;

                    if (mapClick)
                        MoveMap(&ev.motion,mouseDownX, mouseDownY,&fullMap,&mouseDownMapRect,&mapRect);

                    break;
                case SDL_MOUSEBUTTONUP:
                    if (ev.button.button == SDL_BUTTON_LEFT)
                    {
                        mapClick=false;
                    }
                    else if(ev.button.button == SDL_BUTTON_RIGHT)
                    {
                        if (mapRightClicked && !IsInsideBounds(&controlPanelBounds,ev.button.x,ev.button.y))
                        {
                            if (HandleRightCLick(inputType,mapRect,&artyPos,&targetPos,ev.button.x,ev.button.y,xPad,yPad,&env))
                            {
                                StartSimulation(simSpeed.value,&simState);
                            }

                            RefreshCalculationResults(&artyPos, &targetPos, &artyData, &env, resultFields, &deltaX, &deltaZ);
                            UpdateTrajectoryInfoIfNeeded(artyPos,&env,&trInfo,azimuth.value,launchSpeed.value,verticalAngle.value);

                        }
                        mapRightClicked = false;
                    }
                    break;

            }
        }

        SDL_SetRenderDrawColor(renderer, 0,0,0,255);
        SDL_RenderClear(renderer);

        //render map
        SDL_RenderCopy(renderer,terrainTexture,&mapRect,&mapBoxRect);


        //render separator lines
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, 300,0, 300,600);
        SDL_RenderDrawLine(renderer, 299,0, 299,600);



        int pDiff = (int)(300./mapRect.w);

        //render projectile path
        ProcessSimulation(renderer,&trInfo,mapRect,xPad,yPad,pDiff,simSpeed.value,&simState,artyPos,DegToRad(azimuth.value),launchSpeed.value,DegToRad(verticalAngle.value));


        RenderArtyAndTarget(renderer,mapRect,artyPos,targetPos,xPad,yPad,pDiff);

        RenderImpactLocation(renderer,simState.started,trInfo,mapRect,xPad,yPad,pDiff);

        RenderImpactLocationText(renderer,&env,simState.started,trInfo,targetPos);


        RenderLengthReference(renderer,mapRect);
        RenderPositionData(renderer,inputType,artyPos,targetPos, &env);

        RenderDoubleInputField(renderer, &azimuth);
        RenderDoubleInputField(renderer, &verticalAngle);
        RenderDoubleInputField(renderer, &launchSpeed);
        RenderSimSpeedInputField(renderer,&simSpeed);

        SDL_SetRenderDrawColor(renderer,50,50,50,255);
        SDL_RenderDrawRect(renderer,&shootButtonBounds);
        stringRGBA(renderer,(short)(shootButtonBounds.x+shootButtonBounds.w/2 - 24), (short)(shootButtonBounds.y+8),"Shoot!",255,255,255,255);


        RenderTargetDiffAndAngleOptions(renderer,artyPos,targetPos,artyData,resultFields,deltaX,deltaZ);


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
