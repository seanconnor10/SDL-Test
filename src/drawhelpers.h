#include "mathhelpers.h"

v2_int_array *plotLine(int x1, int y1, int x2, int y2, uint32_t *pix);
void myPlotLine(int x1, int y1, int x2, int y2, uint32_t *pix);
v2_int_array *plotLinePoorly(int x1, int y1, int x2, int y2);

#ifndef DRAW_HELPERS
    #define DRAW_HELPERS
    #include "drawhelpers.c"
#endif