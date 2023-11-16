#include "dynarr.h"
#include <stdlib.h>



DynamicArray CreateDynamicArray()
{
    return CreateDynamicArrayWithCapacity(10);
}

DynamicArray CreateDynamicArrayWithCapacity(int capacity)
{
    DynamicArray arr;
    arr.capacity = capacity;
    arr.count = 0;
    arr.data = malloc(arr.capacity*sizeof(double));
    return arr;
}

void ResizeDynamicArray(DynamicArray* arr, int newsize)
{
    arr->capacity = newsize;
    arr->data = realloc(arr->data, arr->capacity*sizeof(double));
    if (arr->capacity < arr->count)
        arr->count = arr->capacity;
}

void FreeDynamicArray(DynamicArray* arr)
{
    free(arr->data);
    arr->capacity = -1;
    arr->count = -1;
}
