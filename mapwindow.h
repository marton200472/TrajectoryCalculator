//
// Created by marton on 2023.10.28..
//

#ifndef NHF_MAPWINDOW_H
#define NHF_MAPWINDOW_H
#include <stdbool.h>
#include <pthread.h>
#include "environment.h"

typedef struct MapWindowData {
    Environment* environment;
    bool quitting;
} MapWindowData;

void RenderMapWindow(MapWindowData* data);
void InitMapWindow(MapWindowData* data);

void QuitMapWindow();
#endif //NHF_MAPWINDOW_H
