#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "Environment.h"

SDL_Window *window;
SDL_Renderer *renderer;

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Trajectory Calculator",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, 0);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderClear(renderer);


    SDL_RenderPresent(renderer);


    bool quit = false;
    SDL_Event ev;
    //main event loop
    while (SDL_WaitEvent(&ev) && ev.type != SDL_QUIT)
    {
        switch (ev.type)
        {
        }
        int **env=GenerateRandomEnvironment(80,60);

        for (int i = 0; i < 80; ++i) {
            for (int j = 0; j < 60; ++j) {
                SDL_Rect r = {10*i,10*j,10,10};
                SDL_SetRenderDrawColor(renderer,env[i][j],env[i][j],env[i][j],255);
                SDL_RenderFillRect(renderer,&r);
            }
        }

        SDL_RenderPresent(renderer);
        for (int i = 0; i < 80; ++i) {
            free(env[i]);
        }
        free(env);
        SDL_Delay(1000);
    }

    SDL_Quit();
    return 0;
}
