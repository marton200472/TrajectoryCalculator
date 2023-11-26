
#ifndef NHF_RENDER_H
#define NHF_RENDER_H
#include "controlpanel.h"
#include "trajectory.h"
#include "simulation.h"

void RenderPositionData(SDL_Renderer *renderer, enum InputType inputType, Point artyPos, Point targetPos, Environment *env);
void RenderLengthReference(SDL_Renderer *renderer, SDL_Rect mapRect);

void RenderDoubleInputField(SDL_Renderer *renderer, DoubleInputField *inputField);
void RenderSimSpeedInputField(SDL_Renderer *renderer, SimSpeedInputField *inputField);
void RenderLoadingScreen(SDL_Renderer *renderer);

SDL_Texture *
RenderEnvironmentToTexture(SDL_Renderer *renderer, Environment *env, SDL_Rect *mapRect, int *xpadding, int *ypadding);

void RenderArtyAndTarget(SDL_Renderer *renderer,SDL_Rect mapRect, Point artyPos, Point targetPos, int xPad, int yPad, double pDiff);
void RenderImpactLocation(SDL_Renderer *renderer, bool simulationStarted, TrajectoryInfo trInfo, SDL_Rect mapRect, int xPad, int yPad, double pDiff);
void RenderImpactLocationText(SDL_Renderer *renderer,Environment *env,bool simulationStarted, TrajectoryInfo trInfo, Point targetPos);
void RenderTargetDiffAndAngleOptions(SDL_Renderer *renderer, Point artyPos, Point targetPos, ArtilleryData artyData, ResultDisplayField *resultFields, double deltaX, int deltaZ );

#endif //NHF_RENDER_H
