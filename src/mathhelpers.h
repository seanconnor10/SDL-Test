double dist(double x1, double y1, double x2, double y2);
double lerp_D(double val1, double val2, double factor);
uint8_t lerp_8_D(uint8_t val1, uint8_t val2, double factor);

#ifndef MATH_HELPERS
    #define MATH_HELPERS
    #include "mathhelpers.c"
#endif