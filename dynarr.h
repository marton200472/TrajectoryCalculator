

#ifndef NHF_DYNARR_H
#define NHF_DYNARR_H

typedef struct DynamicArray {
    int count, capacity;
    double *data;
} DynamicArray;

DynamicArray CreateDynamicArray();
DynamicArray CreateDynamicArrayWithCapacity(int capacity);

void ResizeDynamicArray(DynamicArray* arr, int newsize);

void FreeDynamicArray(DynamicArray* arr);

#endif //NHF_DYNARR_H
