typedef struct VEC2_INT {
    int x;
    int y;
} v2_int;

typedef struct VEC2_INT_ARRAY {
    v2_int *address;
    int length;
} v2_int_array;

double dist(double x1, double y1, double x2, double y2) {
    return sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1))  );    
}

double lerp_D(double val1, double val2, double factor) {
    return val1 + factor*(val2-val1);
}

uint8_t lerp_8_D(uint8_t val1, uint8_t val2, double factor) {
    return (uint8_t) (val1 + factor*(val2-val1));
}