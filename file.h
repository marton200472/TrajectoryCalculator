#ifndef NHF_FILE_H
#define NHF_FILE_H


#include "dynarr.h"
#include "environment.h"

DynamicArray ReadArtilleryV0Data(const char* filename);
void WriteEnvToFile(Environment *e, const char *fileName);
Environment ReadEnvFromFile(const char *fileName);

#endif //NHF_FILE_H
