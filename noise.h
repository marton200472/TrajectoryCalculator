#ifndef NHF_NOISE_H
#define NHF_NOISE_H


#include "vector2.h"

double GetPointValue(double** base, int xpos, int ypos, int scale);
double ** GenerateNoiseBase(int w, int h, int scale);


double Smootherstep(double x);
#endif //NHF_NOISE_H