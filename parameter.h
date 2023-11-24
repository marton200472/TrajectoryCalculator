#ifndef NHF_PARAMETER_H
#define NHF_PARAMETER_H
#include "stdio.h"
#include <stdbool.h>
#include "string.h"

bool ProcessCommandLineArgs(int argc, char *argv[], bool *randomMap, bool *saveMap, int *rW, int *rH, int *mapPathIndex, int *artyDataFileIndex);
#endif //NHF_PARAMETER_H
