#include "tri.h"

v3_t triGetNormal(tri_t tri)
{
    v3_t v01 = v3Sub(tri.p[1], tri.p[0]);
    v3_t v02 = v3Sub(tri.p[2], tri.p[0]);
    return v3Normalize(v3Cross(v01, v02));
}