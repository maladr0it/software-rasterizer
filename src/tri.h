#ifndef TRI_H
#define TRI_H

#include "v2.h"
#include "v3.h"

typedef struct tri
{
    v3_t p[3];
    v2_t t[3];
} tri_t;

v3_t tri_getNormal(tri_t tri);

int tri_clipAgainstPlane(tri_t *outTris, v3_t planePoint, v3_t planeNormal, tri_t inTri);

#endif