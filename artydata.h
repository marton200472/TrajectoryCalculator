#ifndef NHF_ARTYDATA_H
#define NHF_ARTYDATA_H

typedef struct ArtilleryData {
    double minAngle, maxAngle;
    int optionCount;
    double *options;
} ArtilleryData;

void FreeArtilleryData(ArtilleryData* arr);

#endif //NHF_ARTYDATA_H
