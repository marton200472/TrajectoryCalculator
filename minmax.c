#include "minmax.h"

int max(int a, int b) {
    return a < b ? b : a;
}

int min(int a, int b) {
    return a > b ? b : a;
}

