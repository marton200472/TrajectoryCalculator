#include "artydata.h"
#include <stdlib.h>



void FreeArtilleryData(ArtilleryData *arr) {
    free(arr->options);
    arr->optionCount = -1;
}
