
#include "trajectorywindow.h"
#include "environment.h"
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <stdbool.h>
#include <pthread.h>

SDL_Window* trajectoryWindow;
SDL_Renderer* trajectoryRenderer;

void InitTrajectoryWindow(TrajectoryWindowData *data) {
    trajectoryWindow = SDL_CreateWindow("Trajectory Calculator - Data", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    trajectoryRenderer = SDL_CreateRenderer(trajectoryWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_SetRenderDrawColor(trajectoryRenderer, 0, 0, 0, 255);
    SDL_RenderClear(trajectoryRenderer);
    SDL_RenderPresent(trajectoryRenderer);
}

void RenderTrajectoryWindow(TrajectoryWindowData *data) {
    SDL_RenderClear(trajectoryRenderer);
    SDL_RenderPresent(trajectoryRenderer);
}






