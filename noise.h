#ifndef NHF_NOISE_H
#define NHF_NOISE_H


double** GeneratePerlinNoise(int w, int h, int scale);

//smootherstep function suggested by Ken Perlin, https://en.wikipedia.org/wiki/Smoothstep#Variations
double Smootherstep(double x);
#endif //NHF_NOISE_H
