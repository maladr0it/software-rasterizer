#include "tri.h"

v3_t tri_getNormal(tri_t tri)
{
    v3_t v01 = v3_sub(tri.p[1], tri.p[0]);
    v3_t v02 = v3_sub(tri.p[2], tri.p[0]);
    return v3_normalize(v3_cross(v01, v02));
}