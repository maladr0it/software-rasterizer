#include <math.h>
#include "v3.h"

v3_t v3Add(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.y + b.y;
    return result;
}

v3_t v3Sub(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

v3_t v3Mul(v3_t a, float factor)
{
    v3_t result;
    result.x = a.x * factor;
    result.y = a.y * factor;
    result.z = a.z * factor;
    return result;
};

v3_t v3Normalize(v3_t a)
{
    float magnitude = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
    return v3Mul(a, 1 / magnitude);
}

v3_t v3Cross(v3_t a, v3_t b)
{
    v3_t result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

float v3Dot(v3_t a, v3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
