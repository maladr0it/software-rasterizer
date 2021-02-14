#ifndef V3_H
#define V3_H

typedef struct v3
{
    float x;
    float y;
    float z;
} v3_t;

v3_t v3Add(v3_t a, v3_t b);

v3_t v3Sub(v3_t a, v3_t b);

v3_t v3Mul(v3_t a, float factor);

v3_t v3Normalize(v3_t a);

v3_t v3Cross(v3_t a, v3_t b);

float v3Dot(v3_t a, v3_t b);
#endif