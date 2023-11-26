
#include <SDL_render.h>
#include <SDL2_gfxPrimitives.h>
#include "render.h"
#include "controlpanel.h"
#include "trajectory.h"
#include "colortools.h"
#include "simulation.h"
#include "bounds.h"

#ifdef __unix__
#include <pthread.h>

#endif

/**
 * Kiírja a vezérlőpanelre a löveg és a célpont koordinátáit
 * @param renderer a fő ablakhoz tartozó renderer
 * @param inputType az aktív bemeneti mód
 * @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param env a domborzat, amiben számolunk
 * @author Márton
 */
void
RenderPositionData(SDL_Renderer *renderer, enum InputType inputType, Point artyPos, Point targetPos, Environment *env) {
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

/**
 * Hosszúság viszonyítási vonalat jelenít meg a térképen
 * @param renderer a fő ablakhoz tartozó renderer
 * @param mapRect a térképen megjelenített terület
 * @author Márton
 */
void RenderLengthReference(SDL_Renderer *renderer, SDL_Rect mapRect) {
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

/**
 * Megjelenít egy double értékkel rendelkező bemeneti mezőt
 * @param renderer a fő ablakhoz tartozó renderer
 * @param inputField a bemeneti mező
 * @author Márton
 */
void RenderDoubleInputField(SDL_Renderer *renderer, DoubleInputField *inputField) {
    SDL_SetRenderDrawColor(renderer,50,50,50,255);
    SDL_RenderDrawRect(renderer,&inputField->bounds);
    char buf[100];
    sprintf(buf, "%s: %.2lf%s",inputField->name,inputField->value, inputField->unit);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w/2 - strlen(buf)/2*8), (short)(inputField->bounds.y+inputField->bounds.h/2-3), buf,255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + 5), (short)(inputField->bounds.y+inputField->bounds.h/2-3), "<",255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w - 13), (short)(inputField->bounds.y+inputField->bounds.h/2-3), ">",255,255,255,255);
}

/**
 * Megjeleníti a szimulációs sebességhez tartozó bemeneti mezőt
 * @param renderer a fő ablakhoz tartozó renderer
 * @param inputField a bemeneti mező
 * @author Márton
*/
void RenderSimSpeedInputField(SDL_Renderer *renderer, SimSpeedInputField *inputField) {
    SDL_SetRenderDrawColor(renderer,50,50,50,255);
    SDL_RenderDrawRect(renderer,&inputField->bounds);
    char buf[100];
    sprintf(buf, "Simulation: %s",inputField->options[inputField->value]);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w/2 - strlen(buf)/2*8), (short)(inputField->bounds.y+inputField->bounds.h/2-3), buf,255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + 5), (short)(inputField->bounds.y+inputField->bounds.h/2-3), "<",255,255,255,255);
    stringRGBA(renderer,(short)(inputField->bounds.x + inputField->bounds.w - 13), (short)(inputField->bounds.y+inputField->bounds.h/2-3), ">",255,255,255,255);
}

/**
 * Megjeleníti a töltőképernyőt
 * @param renderer a fő ablakhoz tartozó renderer
 * @author Márton
*/
void RenderLoadingScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    stringRGBA(renderer, 500, 300, "Loading...", 255, 255, 255, 255);
    SDL_RenderPresent(renderer);
}


/**
 * Megjeleníti a löveget és a célpontot a térképen
 * @param renderer a fő ablakhoz tartozó renderer
 * @param mapRect a térképen megjelenített terület
 * @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param xPad a térkép x belső margója
 * @param yPad a térkép y belső margója
 * @param pDiff pixel eltolás, hogy a renderelt jel a cél négyzet közepén jelenjen meg
 * @author Márton
*/
void RenderArtyAndTarget(SDL_Renderer *renderer, SDL_Rect mapRect, Point artyPos, Point targetPos, int xPad, int yPad,
                         double pDiff) {
    if (IsInsideBounds(&mapRect, artyPos.x+xPad, artyPos.y+yPad))
    {
        Point pos = {(int)(301 + 800. / mapRect.w * (artyPos.x + xPad - mapRect.x)), (int)(600. / mapRect.h * (artyPos.y + yPad - mapRect.y))};

        boxRGBA(renderer, (short)(pos.x - 5 + pDiff), (short)(pos.y - 5 + pDiff), (short)(pos.x + 5 + pDiff), (short)(pos.y + 5 + pDiff), 250, 165, 0, 255);
        rectangleRGBA(renderer, (short)(pos.x - 5 + pDiff), (short)(pos.y - 5 + pDiff), (short)(pos.x + 5 + pDiff+1), (short)(pos.y + 5 +pDiff+1), 0, 0, 0, 255);
    }

    if (IsInsideBounds(&mapRect, targetPos.x+xPad, targetPos.y+yPad))
    {
        Point pos = {(int)(301 +800./mapRect.w*(targetPos.x + xPad - mapRect.x) + pDiff), (int)(600. / mapRect.h * (targetPos.y + yPad - mapRect.y) + pDiff)};
        filledCircleRGBA(renderer,(short)pos.x, (short)pos.y,5,255,0,0,255);
        circleRGBA(renderer, (short)pos.x, (short)pos.y, 5, 0,0,0,255);
    }
}


/**
* Kiírja, hol csapódik be a lövedék
* @param renderer a fő ablakhoz tartozó renderer
* @param env a domborzat, amiben számolunk
 * @param simulationStarted elkezdődött-e a szimuláció
 * @param trInfo a röppálya adatai
 * @param targetPos a célpont pozíciója
* @author Márton
*/
void RenderImpactLocationText(SDL_Renderer *renderer, Environment *env, bool simulationStarted, TrajectoryInfo trInfo,
                              Point targetPos) {
    char buf[100];
    if (simulationStarted)
    {
        sprintf(buf, "Shell splash @ %d %d", (int)trInfo.pointOfImpact.x, (int)trInfo.pointOfImpact.y);
        if(trInfo.pointOfImpact.x < 0 || trInfo.pointOfImpact.x >= env->w || trInfo.pointOfImpact.y < 0 || trInfo.pointOfImpact.y >= env->h) {
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
}


/**
* Megjeleníti a térképen a becsapódás helyét
* @param renderer a fő ablakhoz tartozó renderer
* @param simulationStarted elkezdődött-e a szimuláció
 * @param trInfo a röppálya adatai
 * @param mapRect a térképen megjelenített terület
 * @param xPad a térkép x belső margója
 * @param yPad a térkép y belső margója
 * @param pDiff pixel eltolás, hogy a renderelt jel a cél négyzet közepén jelenjen meg
* @author Márton
*/
void
RenderImpactLocation(SDL_Renderer *renderer, bool simulationStarted, TrajectoryInfo trInfo, SDL_Rect mapRect, int xPad,
                     int yPad, double pDiff)
                     {
    if (simulationStarted && trInfo.pointOfImpact.x + xPad >= mapRect.x && trInfo.pointOfImpact.x + xPad <= mapRect.x + mapRect.w &&
        trInfo.pointOfImpact.y + yPad >= mapRect.y && trInfo.pointOfImpact.y + yPad <= mapRect.y + mapRect.h)
    {
        Point pos = {(int) (301 + 800. / mapRect.w * (trInfo.pointOfImpact.x + xPad - mapRect.x) + pDiff),
                     (int) (600. / mapRect.h * (trInfo.pointOfImpact.y + yPad - mapRect.y) + pDiff)};
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderDrawLine(renderer,pos.x-5,pos.y-5,pos.x+5,pos.y+5);
        SDL_RenderDrawLine(renderer,pos.x-5,pos.y+5,pos.x+5,pos.y-5);
    }
}

/**
* Megejelíti a löveg és a cél távolságát, és a cél eltalálásához a lehetséges tüzelési paramétereket
* @param renderer a fő ablakhoz tartozó renderer
* @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param artyData a löveg tulajdonságai
 * @param resultFields a megjelenítendő tüzelési paraméterek
 * @param deltaX vízszintes távolság a löveg és a cél között
 * @param deltaZ függőleges távolság a löveg és a cél között
* @author Márton
*/
void RenderTargetDiffAndAngleOptions(SDL_Renderer *renderer, Point artyPos, Point targetPos, ArtilleryData artyData,
                                     ResultDisplayField *resultFields, double deltaX, int deltaZ) {
    char buf[100];
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
}

//struktúra belső használatra
typedef struct Section {
    unsigned *target;
    int start, end;
    int xpad, ypad;
    Environment *env;
} Section;

/**
* Egy térkép szekciót renderel SDL_Surface-be, több szálas műveletekhez
* @param vargp szálnak átadott információ
* @author Márton
*/
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



/**
* Kirenderel egy domborzatot egy SDL_Texture-be, Unix alatt több szálon
* @param renderer a fő ablakhoz tartozó renderer
* @param env a renderelendő domborzat
 * @param mapRect a térképen megjelenített terület, ezt beállítja a függvény
 * @param xpadding x belső margó a térképen
 * @param ypadding y belső margó a térképen
* @return a renderelt textúra
* @author Márton
*/
SDL_Texture *
RenderEnvironmentToTexture(SDL_Renderer *renderer, Environment *env, SDL_Rect *mapRect, int *xpadding, int *ypadding) {
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

