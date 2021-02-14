#ifndef TRI_H
#define TRI_H

#include "v3.h"

typedef struct tri
{
    v3_t p[3];
} tri_t;

v3_t tri_getNormal(tri_t tri);

#endif