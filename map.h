#ifndef NHF_MAP_H
#define NHF_MAP_H
#include "controlpanel.h"

void ZoomMap(SDL_MouseWheelEvent *ev,const SDL_Rect *max_size, SDL_Rect* mapRect, int x, int y);
void MoveMap(SDL_MouseMotionEvent *ev, int startX, int startY, const SDL_Rect *max_size,SDL_Rect *startMapRect, SDL_Rect *mapRect);
bool HandleRightCLick(enum InputType inputType, SDL_Rect mapRect, Point *artyPos, Point *targetPos, int mouseX, int mouseY, int xPad, int yPad,Environment *env);
#endif //NHF_MAP_H
