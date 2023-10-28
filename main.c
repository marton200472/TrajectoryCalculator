#include <stdio.h>
#include <SDL.h>
#include "environment.h"
#include "debugmalloc.h"
#include "2darr.h"
#include "mapwindow.h"
#include "trajectorywindow.h"


int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    Environment env = GenerateRandomEnvironment(800,600);
    MapWindowData mapData = {.environment = &env, .quitting = false};
    TrajectoryWindowData trajectoryData = {.environment = &env, .quitting = false};

    InitMapWindow(&mapData);
    InitTrajectoryWindow(&trajectoryData);


    bool quit = false;
    SDL_Event ev;
    while (!quit)
    {
        while (SDL_PollEvent(&ev) && !quit)
        {
            switch (ev.type) {
                case SDL_WINDOWEVENT:
                    if (ev.window.event == SDL_WINDOWEVENT_CLOSE)
                        quit = true;
                    break;
            }
        }
        RenderMapWindow(&mapData);
        RenderTrajectoryWindow(&trajectoryData);
        SDL_Delay(10);
    }

    SDL_Quit();

    FreeEnvironment(&env);
    return 0;
}
