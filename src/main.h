#ifndef MAIN_H
#define MAIN_H

typedef struct v3
{
    float x;
    float y;
    float z;
} v3_t;

typedef struct tri
{
    v3_t p[3];
} tri_t;

#endif