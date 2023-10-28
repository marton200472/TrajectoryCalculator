
#ifndef NHF_TRAJECTORYWINDOW_H
#define NHF_TRAJECTORYWINDOW_H

#include <stdbool.h>
#include "environment.h"
#include <pthread.h>

typedef struct TrajectoryWindowData {
    Environment* environment;
    bool quitting;
} TrajectoryWindowData;

void InitTrajectoryWindow(TrajectoryWindowData* data);
void RenderTrajectoryWindow(TrajectoryWindowData* data);
#endif //NHF_TRAJECTORYWINDOW_H
