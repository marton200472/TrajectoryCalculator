#include "colortools.h"


/**
 * Magassághoz RGB kódokat rendel (domborzati térképhez)
 * @param height magasság az adott pontban
 * @return a magassághoz rendelt szín 0x00RRGGBB formátumban
 * @author Márton
 */
unsigned height_to_rgb(int height) {
    if (height < 0)
        return 0x15b2ff;
    if (height < 100)
        return 0x86c979;
    if (height < 150)
        return 0xb1d166;
    if (height < 200)
        return 0xd9e18e;
    if (height < 300)
        return 0xd9e18e;
    if (height < 500)
        return 0xf9cd8c;
    if (height < 700)
        return 0xf9ae75;
    if (height < 1000)
        return 0xd19c70;

    return 0xb28f71;
}
