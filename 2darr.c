#include "2darr.h"
#include "debugmalloc.h"
#include <stdlib.h>

/**
 * Lefoglal egy adott méretű 2D-s dinamikus tömböt
 * @param x első dimenzió mérete
 * @param y második dimenzió mérete
 * @param element_size egy tömbelem mérete byte-ban
 * @return pointer a tömbre
 * @author Márton
 */
void **Allocate2DArr(int x, int y, size_t element_size) {
    void** arr = malloc(x*sizeof(void*));
    for (int i = 0; i < x; ++i) {
        arr[i] = malloc(y*element_size);
    }
    return arr;
}

/**
 * Felszabadít egy adott méretű 2D-s dinamikus tömböt
 * @param p pointer a felszabadítandó tömbre
 * @param dim1 az első dimenzió mérete
 * @author Márton
 */
void Free2DArr(void **p, int dim1) {
    for (int i = 0; i < dim1; ++i) {
        free(p[i]);
    }
    free(p);
}
