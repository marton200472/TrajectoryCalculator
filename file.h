#ifndef NHF_FILE_H
#define NHF_FILE_H


#include "artydata.h"
#include "environment.h"

ArtilleryData ReadArtilleryData(const char* filename);
void WriteEnvToFile(Environment *e, const char *fileName);
Environment ReadEnvFromFile(const char *fileName);

#endif //NHF_FILE_H
