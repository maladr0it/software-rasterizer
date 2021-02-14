#ifndef V3_H
#define V3_H

typedef struct v3
{
    float x;
    float y;
    float z;
} v3_t;

v3_t v3_add(v3_t a, v3_t b);

v3_t v3_sub(v3_t a, v3_t b);

v3_t v3_mul(v3_t a, float factor);

v3_t v3_div(v3_t a, float b);

v3_t v3_cross(v3_t a, v3_t b);

float v3_dot(v3_t a, v3_t b);

float v3_len(v3_t a);

v3_t v3_normalize(v3_t a);

#endif