#include <SDL_events.h>
#include "controlpanel.h"
#include "point.h"
#include "artydata.h"
#include "environment.h"
#include "trajectory.h"
#include "bounds.h"
#include "simulation.h"


/**
 * Frissíti a célpont eltalálásához lehetséges szögeket
 * @param artyPos a löveg pozíciója
 * @param targetPos a cél pozíciója
 * @param artyData a löveg adatai
 * @param env a domborzat, amiben a számításokat végezzük
 * @param resultFields pointer, itt tárolódnak dinamikus memóriaterületen a megjelenítendő eredmények (felülírja a tartalmát a függvény)
 * @param dX a célpont és a löveg vízszintes síkon vett távolsága
 * @param dZ a célpont és a löveg függőleges távolsága
 * @author Márton
 */
void RefreshCalculationResults(Point *artyPos, Point *targetPos, ArtilleryData *artyData, Environment *env,
                               ResultDisplayField *resultFields, double *dX, int *dZ) {
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


/**
 * A nyíl gombok lenyomását kezeli
 * @param ev a kezelendő esemény
 * @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param inputType az aktív bemeneti mód
 * @param env a domborzat, amiben a számításokat végezzük
 * @author Márton
 */
void HandleArrowKeys(SDL_KeyboardEvent ev, Point *artyPos, Point *targetPos, enum InputType inputType, Environment *env) {
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

/**
 * A bemeneti mezőkön való görgetést kezeli
 * @param azimuth a löveg azimutjához tartozó bementi mező
 * @param verticalAngle a löveg vertikális szögéhez tartozó bementi mező
 * @param launchSpeed a lövedék kezdősebességéhez tartozó bementi mező
 * @param simSpeed a szimuláció sebességét beállító bementi mező
 * @param artyData a löveg tulajdonságai
 * @param ev a kezelendő esemény
 * @param mouseX a kurzor x koordinátája
 * @param mouseY a kurzor y koordinátája
 * @return true, ha újra kell indítani a szimulációt, mert változott valamilyen paraméter, különben false
 * @author Márton
 */
bool HandleScroll(DoubleInputField *azimuth, DoubleInputField *verticalAngle, DoubleInputField *launchSpeed,
                  SimSpeedInputField *simSpeed, ArtilleryData artyData, SDL_MouseWheelEvent ev, int mouseX, int mouseY) {
    if (IsInsideBounds(&azimuth->bounds, mouseX,mouseY))
    {
        azimuth->value = round((azimuth->value + ev.y*2.));
        while (azimuth->value >= 360.)
            azimuth->value -= 360.;
        while (azimuth->value < 0)
            azimuth->value+=360.;

    }
    else if (IsInsideBounds(&verticalAngle->bounds, mouseX,mouseY))
    {
        verticalAngle->value = round((verticalAngle->value + ev.y*2.));
        if (verticalAngle->value < artyData.minAngle)
            verticalAngle->value = artyData.minAngle;
        else if(verticalAngle->value > artyData.maxAngle)
            verticalAngle->value = artyData.maxAngle;
    }
    else if (IsInsideBounds(&launchSpeed->bounds,mouseX,mouseY))
    {
        int currentIndex = 0;
        for (int i = 0; i < artyData.optionCount; ++i) {
            if (artyData.options[i] == launchSpeed->value)
            {
                currentIndex = i;
                break;
            }
        }
        currentIndex+=ev.y;
        if (currentIndex >= artyData.optionCount)
            currentIndex=0;
        else if (currentIndex < 0)
            currentIndex = artyData.optionCount-1;
        launchSpeed->value = artyData.options[currentIndex];
    }
    else if(IsInsideBounds(&simSpeed->bounds, mouseX,mouseY))
    {
        simSpeed->value+=ev.y;
        if (simSpeed->value < 0)
            simSpeed->value = SimSpeed_OptionCount-1;
        else if(simSpeed->value >= SimSpeed_OptionCount)
            simSpeed->value = 0;
    }
    else
        return false;

    return true;
}


/**
 * A vezérlőpanel kattintás eseményét kezeli
 * @param azimuth a löveg azimutjához tartozó bementi mező
 * @param verticalAngle a löveg vertikális szögéhez tartozó bementi mező
 * @param launchSpeed a lövedék kezdősebességéhez tartozó bementi mező
 * @param simSpeed a szimuláció sebességét beállító bementi mező
 * @param shootButtonBounds a lövés gomb területe
 * @param resultFields számítási eredmény mezők
 * @param artyData a löveg tulajdonságai
 * @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param selectedResult a kiválasztott meghajtótöltet indexe
 * @param selectedAngle a kiválasztott szög indexe a meghajtótölttel használható tüzelési szögek belül
 * @param mouseX a kurzor x koordinátája
 * @param mouseY a kurzor y koordinátája
 * @return true, ha újra kell indítani a szimulációt, mert változott valamilyen paraméter, különben false
 * @author Márton
 */
bool HandleLeftClick(DoubleInputField *azimuth, DoubleInputField *verticalAngle, DoubleInputField *launchSpeed,
                     SimSpeedInputField *simSpeed, SDL_Rect *shootButtonBounds, ResultDisplayField *resultFields,
                     Point artyPos, Point targetPos, int *selectedResult, int *selectedAngle, ArtilleryData artyData,
                     int mouseX, int mouseY) {
    for (int i = 0; i < artyData.optionCount; ++i) {
        if (IsInsideBounds(&resultFields[i].bounds, mouseX, mouseY) && resultFields[i].angleResult.resultCount > 0)
        {
            if (*selectedResult != i)
            {
                *selectedResult = i;
                *selectedAngle = 0;
            }
            else
            {
                (*selectedAngle)++;
                if (*selectedAngle == resultFields[i].angleResult.resultCount)
                    *selectedAngle = 0;
            }
            verticalAngle->value = resultFields[i].angleResult.results[*selectedAngle];
            launchSpeed->value = artyData.options[i];

            double az = 360. - RadToDeg(atan2(artyPos.y - targetPos.y, targetPos.x - artyPos.x) - M_PI_2);
            while (az >= 360.)
                az -= 360.;
            while (az < 0.)
                az += 360.;
            azimuth->value = az;
            return true;
        }
    }

    if (IsInsideBounds(&azimuth->bounds, mouseX, mouseY))
    {
        int dir = mouseX < azimuth->bounds.x+azimuth->bounds.w/2 ? -1 : 1;
        azimuth->value = round((azimuth->value + dir*0.5)*10.)/10.;
        while (azimuth->value >= 360.)
            azimuth->value -= 360.;
        while (azimuth->value < 0)
            azimuth->value+=360.;
    }
    else if (IsInsideBounds(&verticalAngle->bounds, mouseX, mouseY))
    {
        int dir = mouseX < verticalAngle->bounds.x+verticalAngle->bounds.w/2 ? -1 : 1;
        verticalAngle->value = round((verticalAngle->value + dir*0.5)*10.)/10.;
        if (verticalAngle->value < artyData.minAngle)
            verticalAngle->value = artyData.minAngle;
        else if(verticalAngle->value > artyData.maxAngle)
            verticalAngle->value = artyData.maxAngle;
    }
    else if (IsInsideBounds(&launchSpeed->bounds,mouseX, mouseY))
    {
        int dir = mouseX < launchSpeed->bounds.x+launchSpeed->bounds.w/2 ? -1 : 1;
        int currentIndex = 0;
        for (int i = 0; i < artyData.optionCount; ++i) {
            if (artyData.options[i] == launchSpeed->value)
            {
                currentIndex = i;
                break;
            }
        }
        currentIndex+=dir;
        if (currentIndex < 0)
            currentIndex = artyData.optionCount-1;
        currentIndex %= artyData.optionCount;
        launchSpeed->value = artyData.options[currentIndex];
    }
    else if(IsInsideBounds(&simSpeed->bounds, mouseX, mouseY))
    {
        int dir = mouseX < launchSpeed->bounds.x+launchSpeed->bounds.w/2 ? -1 : 1;
        simSpeed->value+=dir;
        if (simSpeed->value < 0)
            simSpeed->value = SimSpeed_OptionCount-1;
        else if(simSpeed->value >= SimSpeed_OptionCount)
            simSpeed->value = 0;
    }
    else if(IsInsideBounds(shootButtonBounds,mouseX, mouseY))
    {
        return true;
    }
    else
        return false;

    return true;
}
