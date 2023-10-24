#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "Environment.h"
#include "debugmalloc.h"
#include <math.h>
#include <time.h>
#include "noise.h"

SDL_Window *window;
SDL_Renderer *renderer;

double fractional_mod(double a, int b){
    double fraction = modf(a,&a);
    return  (int)a%b + fraction;
}

double max(double a, double b){
    return a<b?b:a;
}

double min(double a, double b){
    return a<b?a:b;
}

double f(double n, double h, double s, double v)
{
    double k = fractional_mod(n+h/60,6);
    return v - v*s * max(0, min(min(k, 4 - k), 1));
}

uint32_t hsl_to_rgb(double h, double s, double l)
{
    /*printf("%f\n", f(5,h,s,v));

    return ((int)(f(5, h, s, v) * 255) << 16) | ((int)f(3, h, s, v) << 8) | (int)f(1, h, s, v);
*/

     double c = (1-fabs(2*l-1))*s;
     double h1 = h/60;
     double x = c * (1-fabs(fractional_mod(h1,2)-1));
     double r1, g1, b1;
    switch ((int)h1) {
        case 0:
            r1 = c;
            g1 = x;
            b1 = 0;
            break;
        case 1:
            r1 = x;
            g1 = c;
            b1 = 0;
            break;
        case 2:
            r1 = 0;
            g1 = c;
            b1 = x;
            break;
        case 3:
            r1 = 0;
            g1 = x;
            b1 = c;
            break;
        case 4:
            r1 = x;
            g1 = 0;
            b1 = c;
            break;
        case 5:
            r1 = c;
            g1 = 0;
            b1 = x;
            break;
    }

    double m = l-c/2;
    int r = (r1+m)*255, g = (g1+m)*255, b = (b1+m)*255;
    return r << 16 | g << 8 | b;
}



uint32_t height_to_rgb(int height)
{
    /*if (height < 70)
        return hsl_to_rgb(130-50.*height/70, 0.79-0.3*height/70, 0.2+0.3*height/70);
    if (height < 100)
        return hsl_to_rgb(80,0.49,0.5 - 0.2*(height-70)/30);
    if (height < 140)
        return 0x827504;

    return 0x423703;*/
    double w = height/1000.;
    if (height % 10 == 0)
        return 0;
    return hsl_to_rgb(130-50*(w), 0.7, 0.55 - 0.55 * (1.5*w));
    //return hsl_to_rgb(0,0,1-w);
}

int main(int argc, char *argv[]) {
    debugmalloc_log_file("debugmalloc.log");
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(0));

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
        int **env=GenerateRandomEnvironment(800,600);
        int max = 0;
        int min = 255;
        for (int i = 0; i < 800; ++i) {
            for (int j = 0; j < 600; ++j) {
                if (env[i][j] > max)
                    max = env[i][j];
                if (env[i][j] < min)
                    min = env[i][j];
                uint32_t color = height_to_rgb(env[i][j]);
                //SDL_Rect r = {10*i,10*j,10,10};
                SDL_SetRenderDrawColor(renderer,(color & 0xFF0000) >> 16,(color & 0xFF00) >> 8 ,color & 0xFF,255);
                //SDL_RenderFillRect(renderer,&r);
                SDL_RenderDrawPoint(renderer,i,j);
            }
        }
        printf("%d  %d\n",max,min);

        SDL_RenderPresent(renderer);
        for (int i = 0; i < 800; ++i) {
            free(env[i]);
        }
        free(env);
        //SDL_Delay(1000);
    }

    SDL_Quit();
    return 0;
}
