#ifndef NHF_BOUNDS_H
#define NHF_BOUNDS_H

#include <stdbool.h>
#include <SDL_rect.h>

bool IsInsideBounds(SDL_Rect *bounds, int x, int y);

#endif //NHF_BOUNDS_H
