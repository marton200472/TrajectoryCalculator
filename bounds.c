#include "bounds.h"
#include <SDL_rect.h>
#include <stdbool.h>



/**
 * Eldönti, hogy egy pont egy téglalapon belül van-e
 * @param bounds a téglalap
 * @param x a pont x koordinátája
 * @param y a pont y koordinátája
 * @return true, ha a pont benne van a téglalapban, különben false
 * @author Márton
 */
bool IsInsideBounds(SDL_Rect *bounds, int x, int y) {
    return x >= bounds->x && x < bounds->x + bounds->w && y >= bounds->y && y < bounds->y + bounds->h;
}
