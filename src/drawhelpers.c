#include "mathhelpers.h"
#include <stdint.h>

v2_int_array *plotLine(int x1, int y1, int x2, int y2, uint32_t *pix) {
    int l = abs(x2-x1) + abs(y2-y1) + 1;

    v2_int *points = malloc( l*sizeof(v2_int));
    v2_int_array *arr = malloc( sizeof(v2_int_array) );
    arr->address = points;
    arr->length = l;

    if (x1 > x2) {
        int temp = x2;
        x2 = x1;
        x1 = temp;
    } 

    if (y1 > y2) {
        int temp = y2;
        y2 = y1;
        y1 = temp;
    }
 
    int dx = x2 - x1;
    int dy = y2 - y1;
    int cx = x1 < x2 ? 1 : -1;
    int cy = y1 < y2 ? 1 : -1;
    
    int e = (dx > dy ? dx : -dy) / 2, e2;

    SDL_Color col = {.r = 0xFF, .g = 0x90, .b = 0x10, .a = 0xFF};
    
    for (int i=0; i<l; i++) {
        points[i].x = x1;
        points[i].y = y1;
        
        gameSetPixel(pix, x1, y1, col, 480, 270);

        if (x1 == x2 && y1 == y2) 
            break;

        e2 = e;

        if (e2 > -dx) {
            e -= dy;
            x1 += cx;
        }

        if (e2 < dy) {
            e += dx;
            y1 += cy;
        }
    }
    
    return arr;
}

void myPlotLine(int x1, int y1, int x2, int y2, uint32_t *framePix) {
    int distX = x2-x1;
    int distY = y2-y1;
    float ratioX = distX/distY;
    float ratioY = distY/distX;
    int dirX = x1 < x2 ? 1 : -1;
    int dirY = y1 < y2 ? 1 : -1;

    int e = distX/2, e2;
    float f = 1.0, f2;

    SDL_Color color = {.r = 0xFF, .g = 0x90, .b = 0x10, .a = 0xFF};
    
    gameSetPixel(framePix, x1, y1, color, 480, 270);

    while(x1 != x2 || y1 != y2) {
        f2 = f;

        f2 = f;

        if (f2 > -1) {
            f -= distY;
            x1++;
        }

        if (f2 < 1) {
            f += distX;
            y1++;
        }

        gameSetPixel(framePix, x1, y1, color, 480, 270);

    }
}

v2_int_array *plotLinePoorly(int x1, int y1, int x2, int y2) {
    int l = (x2-x1+1);
    v2_int *points = malloc( l*sizeof(v2_int));
    v2_int_array *arr = malloc( sizeof(v2_int_array) );
    arr->address = points;
    arr->length = l;
    float slope = (y2-y1) / (x2-x1);
    int drawY = y1;
    for (int i=0; i<l; i++) {
        points[i].x=x1+i;
        drawY += slope;
        points[i].y=drawY;
    }
    return arr;
}