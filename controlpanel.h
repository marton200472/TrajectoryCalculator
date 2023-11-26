#ifndef NHF_CONTROLPANEL_H
#define NHF_CONTROLPANEL_H

#include "trajectory.h"
#include "point.h"
#include "simulation.h"

enum InputType {
    Input_None, Input_ArtyPos, Input_TargetPos
};

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

void RefreshCalculationResults(Point *artyPos, Point *targetPos,ArtilleryData *artyData,Environment *env, ResultDisplayField *resultFields, double *dX, int *dZ);

void HandleArrowKeys(SDL_KeyboardEvent ev, Point *artyPos, Point *targetPos, enum InputType inputType, Environment *env);

bool HandleScroll(DoubleInputField *azimuth, DoubleInputField *verticalAngle, DoubleInputField *launchSpeed, SimSpeedInputField *simSpeed, ArtilleryData artyData,SDL_MouseWheelEvent ev, int mouseX, int mouseY);

bool HandleLeftClick(DoubleInputField *azimuth, DoubleInputField *verticalAngle, DoubleInputField *launchSpeed,
                     SimSpeedInputField *simSpeed, SDL_Rect *shootButtonBounds, ResultDisplayField *resultFields,
                     Point artyPos, Point targetPos, int *selectedResult, int *selectedAngle, ArtilleryData artyData,
                     int mouseX, int mouseY);

#endif //NHF_CONTROLPANEL_H
