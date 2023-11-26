#include "minmax.h"

/**
 * Két egész szám közül kiválasztja a nagyobbat
 * @author Márton
 */
int max(int a, int b) {
    return a < b ? b : a;
}

/**
 * Két egész szám közül kiválasztja a kisebbet
 * @author Márton
 */
int min(int a, int b) {
    return a > b ? b : a;
}

