#include "artydata.h"
#include <stdlib.h>


/**
 * Felszabadítja egy ArtylleryData típusú struktúra dinamikusan foglalt memóriaterületét
 * @param arr pointer a felszabadítandó struktúrára
 * @author Márton
 */
void FreeArtilleryData(ArtilleryData *arr) {
    free(arr->options);
    arr->optionCount = -1;
}
