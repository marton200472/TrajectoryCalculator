#include "2darr.h"
#include <stdlib.h>
#include "debugmalloc.h"


void **Allocate2DArr(int x, int y, size_t element_size) {
    void** arr = malloc(x*sizeof(void*));
    for (int i = 0; i < x; ++i) {
        arr[i] = malloc(y*element_size);
    }
    return arr;
}



void Free2DArr(void **p, int dim1) {
    for (int i = 0; i < dim1; ++i) {
        free(p[i]);
    }
    free(p);
}
