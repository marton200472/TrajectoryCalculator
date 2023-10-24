#include <stdlib.h>

#ifndef NHF_2DARR_H
#define NHF_2DARR_H
void** Allocate2DArr(int x, int y, size_t element_size);
void Free2DArr(void** p, int dim1);
#endif //NHF_2DARR_H
