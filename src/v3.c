#include <math.h>
#include "v3.h"

v3_t v3_add(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

v3_t v3_sub(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

v3_t v3_mul(v3_t a, float b)
{
    v3_t result;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    return result;
};

v3_t v3_div(v3_t a, float b)
{
    v3_t result;
    result.x = a.x / b;
    result.y = a.y / b;
    result.z = a.z / b;
    return result;
}

v3_t v3_cross(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

float v3_dot(v3_t a, v3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float v3_len(v3_t a)
{
    return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

v3_t v3_normalize(v3_t a)
{
    return v3_div(a, v3_len(a));
}

v3_t v3_getPlaneIntersect(v3_t planePoint, v3_t planeNormal, v3_t p0, v3_t p1)
{
    float t = v3_dot(v3_sub(p0, planePoint), planeNormal) / v3_dot(v3_sub(p0, p1), planeNormal);
    return v3_add(p0, v3_mul(v3_sub(p1, p0), t));
}

bool v3_isInsideHalfspace(v3_t planePoint, v3_t planeNormal, v3_t point)
{
    return v3_dot(v3_sub(point, planePoint), planeNormal) >= 0;
}
