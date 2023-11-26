//

#include <time.h>
#include <stdbool.h>
#include <SDL_render.h>
#include <SDL2_gfxPrimitives.h>
#include "simulation.h"
#include "trajectory.h"
#include "point.h"
#include "debugmalloc.h"

/**
* Az időt adja vissza másodpercekben
* @return az idő másodpercekben, törtrésszel együtt
* @author Márton
*/
double time_secs()
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return (double)t.tv_sec + (double)t.tv_nsec * 1e-9;
}


/**
* Elindítja a szimulációt
* @param simSpeed a szimuláció sebessége
 * @param simState a szimuláció állapotát tartalmazó struktúra
* @author Márton
*/
void StartSimulation(enum SimSpeed simSpeed, SimulationState *simState) {
    if (simSpeed == SimSpeed_Instant)
    {
        simState->ended = true;
        simState->started = true;
    }
    else{
        simState->startTime = time_secs();
        simState->started = true;
        simState->ended = false;
    }
}


/**
 * Megjeleníti a térképen a szimuláció eredményét, a röppályát
 * @param renderer a fő ablakhoz tartozó renderer
 * @param trInfo a röppálya adatai
 * @param mapRect a térképen megjelenített terület
 * @param xPad a térkép x belső margója
 * @param yPad a térkép y belső margója
 * @param pDiff pixel eltolás, hogy a renderelt jel a cél négyzet közepén jelenjen meg
 * @param simSpeed a szimuláció sebessége
 * @param simState a szimuláció állapota
 * @param artyPos a löveg pozíciója
 * @param azimuth a löveg azimutja radiánban
 * @param v0 a lövedék kezdősebessége m/s-ban
 * @param angle a lövés szöge radiánban
 * @author Márton
 */
void ProcessSimulation(SDL_Renderer *renderer, TrajectoryInfo *trInfo, SDL_Rect mapRect, int xPad, int yPad, double pDiff,
                  enum SimSpeed simSpeed, SimulationState *simState, Point artyPos, double azimuth,
                  double v0, double angle) {
    if (!simState->started)
        return;



    int iters;
    double t = time_secs() - simState->startTime;
    if (simSpeed == SimSpeed_Quick)
        iters = (int)(t * 10) / SIMULATION_TIMESTEP_S;
    else
        iters = (int)t / SIMULATION_TIMESTEP_S;

    if (iters >= trInfo->pointCount)
        simState->ended = true;


    if (simState->ended)
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

    if (!simState->ended)
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


/**
 * Újraszámolja a röppályát, felszabadítja az előzőt
 * @param artyPos a löveg pozíciója
 * @param env a domborzat, amiben számolunk
 * @param trInfo a röppálya adatai
 * @param azimuth a löveg azimutja fokban
 * @param launchSpeed a lövedék kezdősebessége m/s-ban
 * @param verticalAngle a löveg szöge fokban
 * @author Márton
 */
void UpdateTrajectoryInfoIfNeeded(Point artyPos, Environment *env, TrajectoryInfo *trInfo, double azimuth,
                                  double launchSpeed, double verticalAngle) {
    if (artyPos.x != -1 && artyPos.y != -1)
    {
        free(trInfo->points);
        *trInfo = CalculateTrajectory(env, &artyPos, azimuth, launchSpeed, verticalAngle);
    }
}

