#ifndef V2_H
#define V2_H

typedef struct v2
{
    float u;
    float v;
} v2_t;

v2_t v2_add(v2_t a, v2_t b);

v2_t v2_sub(v2_t a, v2_t b);

v2_t v2_mul(v2_t a, float b);

v2_t v2_interpolate(v2_t from, v2_t to, float t);

#endif