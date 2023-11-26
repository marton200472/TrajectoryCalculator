#ifndef NHF_SIMULATION_H
#define NHF_SIMULATION_H

#include <SDL_rect.h>
#include <SDL_render.h>
#include "trajectory.h"

enum SimSpeed {
    SimSpeed_Instant=0,SimSpeed_Quick=1,SimSpeed_RealTime=2, SimSpeed_OptionCount=3
};

typedef struct SimSpeedInputField {
    int value;
    SDL_Rect bounds;
    const char *options[SimSpeed_OptionCount];
} SimSpeedInputField;

typedef struct SimulationState {
    bool started, ended;
    double startTime;
} SimulationState;


void StartSimulation(enum SimSpeed simSpeed, SimulationState *simState);
void ProcessSimulation(SDL_Renderer *renderer, TrajectoryInfo *trInfo, SDL_Rect mapRect, int xPad, int yPad, double pDiff,
                       enum SimSpeed simSpeed, SimulationState *simState, Point artyPos, double azimuth,
                       double v0, double angle);

void UpdateTrajectoryInfoIfNeeded(Point artyPos, Environment *env, TrajectoryInfo *trInfo, double azimuth, double launchSpeed, double verticalAngle);
#endif //NHF_SIMULATION_H
