#include "v2.h"

v2_t v2_add(v2_t a, v2_t b)
{
    v2_t result;
    result.u = a.u + b.u;
    result.v = a.v + b.v;
    return result;
}

v2_t v2_sub(v2_t a, v2_t b)
{
    v2_t result;
    result.u = a.u - b.u;
    result.v = a.v - b.v;
    return result;
}

v2_t v2_mul(v2_t a, float b)
{
    v2_t result;
    result.u = a.u * b;
    result.v = a.v * b;
    return result;
}

v2_t v2_interpolate(v2_t from, v2_t to, float t)
{
    return v2_add(v2_mul(v2_sub(to, from), t), from);
}