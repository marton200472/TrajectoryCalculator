#include <SDL_events.h>
#include <stdbool.h>
#include "map.h"
#include "minmax.h"
#include "controlpanel.h"

/**
 * A térkép nagyítását vezérli (egérgörgő esemény)
 * @param ev a kezelendő egér esemény
 * @param max_size a térkép maximális mérete
 * @param mapRect a térképen megjelenített terület
 * @param x a kurzor x koordinátája
 * @param y a kurzor y koordinátája
 * @author Márton
 */
void ZoomMap(SDL_MouseWheelEvent *ev, const SDL_Rect *max_size, SDL_Rect *mapRect, int x, int y) {
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

/**
 * A térkép mozgatását végzi (kattintás és húzás)
 * @param ev a kezelendő esemény
 * @param startX a kurzor x koordinátája a bal egérgomb lenyomásakor
 * @param startY a kurzor y koordinátája a bal egérgomb lenyomásakor
 * @param startMapRect a térképen megjelenített terület a bal egérgomb lenyomásakor
 * @param mapRect a térképen megjelenített terület
 * @author Márton
 */
void MoveMap(SDL_MouseMotionEvent *ev, int startX, int startY, const SDL_Rect *max_size, SDL_Rect *startMapRect,
             SDL_Rect *mapRect) {
    double multiplier = mapRect->w/800.;

    int nx = (int)(startMapRect->x - (ev->x-startX)*multiplier);
    int ny = (int)(startMapRect->y - (ev->y-startY)*multiplier);

    mapRect->x = min(max(nx, 0), max_size->w-mapRect->w);
    mapRect->y = min(max(ny, 0), max_size->h-mapRect->h);
}


/**
 * A térképen való jobb kattintást kezeli
 * @param inputType az aktív bemenet típus
 * @param mapRect a térképen megjelenített terület
 * @param artyPos a löveg pozíciója
 * @param targetPos a célpont pozíciója
 * @param mouseX a kurzor x koordinátája
 * @param mouseY a kurzor y koordinátája
 * @param xPad x tengely belső margó a térképen
 * @param yPad y tengely belső margó a térképen
 * @param env a domborzat, amivel számolunk
 * @return true, ha újra kell indítani a szimulációt, mert változott valamilyen paraméter, különben false
 * @author Márton
 */
bool
HandleRightCLick(enum InputType inputType, SDL_Rect mapRect, Point *artyPos, Point *targetPos, int mouseX, int mouseY,
                 int xPad, int yPad, Environment *env) {
    if (inputType == Input_ArtyPos)
    {
        Point np = (Point){.x = min(max((int)(mapRect.x + mapRect.w/800.*(mouseX-301) - xPad), 0), env->w - 1), .y = min(max((int)(mapRect.y + mapRect.h / 600. * mouseY - yPad), 0), env->h - 1)};
        if (!GetHeightAtCoordinates(env,np.x, np.y).water)
            *artyPos = np;
        return true;
    }
    else if (inputType == Input_TargetPos)
    {
        *targetPos = (Point){.x = min(max((int)(mapRect.x +mapRect.w/800.*(mouseX-301) - xPad), 0), env->w - 1), .y = min(max((int)(mapRect.y + mapRect.h / 600. * mouseY - yPad), 0), env->h - 1)};
    }
    return false;
}


