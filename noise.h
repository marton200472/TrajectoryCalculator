#ifndef NHF_NOISE_H
#define NHF_NOISE_H

double GetValueAtCoordinates(double** base, int xpos, int ypos, int scale);
double **GenerateNoiseBase(int w, int h, int scale);
double Interpolate(double a, double b, double w);


double Smootherstep(double x);
#endif //NHF_NOISE_H
