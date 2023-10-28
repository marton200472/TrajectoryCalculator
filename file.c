#include "file.h"
#include <stdio.h>


DynamicArray ReadArtilleryV0Data(const char *filename) {
    FILE* f = fopen(filename, "r");
    int c;
    fscanf(f,"%d",&c);
    DynamicArray arr = CreateDynamicArrayWithCapacity(c);
    for (int i = 0; i < c; ++i) {
        fscanf(f,"%lf",arr.data+i);
    }
    fclose(f);
    arr.count = arr.capacity;
    return arr;
}
