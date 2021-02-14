#include <stdlib.h>
#include <math.h>
#include "mat4x4.h"

mat4x4_t mat4x4_createIdentity()
{
    mat4x4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4x4_t mat4x4_createProj(float aspectRatio, float fov, float zNear, float zFar)
{
    mat4x4_t result;

    float f = 1 / tan(fov / 2);
    float q = zFar / (zFar - zNear);

    result.m[0][0] = f * 1 / aspectRatio;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = f;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = q;
    result.m[2][3] = 1;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = -zNear * q;
    result.m[3][3] = 0;

    return result;
}

mat4x4_t mat4x4_createRotX(float theta)
{
    mat4x4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = cosf(theta);
    result.m[1][2] = -sinf(theta);
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = sinf(theta);
    result.m[2][2] = cosf(theta);
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4x4_t mat4x4_createRotY(float theta)
{
    mat4x4_t result;

    result.m[0][0] = cosf(theta);
    result.m[0][1] = 0;
    result.m[0][2] = -sinf(theta);
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = sinf(theta);
    result.m[2][1] = 0;
    result.m[2][2] = cosf(theta);
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4x4_t mat4x4_createRotZ(float theta)
{
    mat4x4_t result;

    result.m[0][0] = cosf(theta);
    result.m[0][1] = -sinf(theta);
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = sinf(theta);
    result.m[1][1] = cosf(theta);
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;

    result.m[3][0] = 0;
    result.m[3][1] = 0;
    result.m[3][2] = 0;
    result.m[3][3] = 1;

    return result;
}

mat4x4_t mat4x4_createTranslate(v3_t d)
{
    mat4x4_t result;

    result.m[0][0] = 1;
    result.m[0][1] = 0;
    result.m[0][2] = 0;
    result.m[0][3] = 0;

    result.m[1][0] = 0;
    result.m[1][1] = 1;
    result.m[1][2] = 0;
    result.m[1][3] = 0;

    result.m[2][0] = 0;
    result.m[2][1] = 0;
    result.m[2][2] = 1;
    result.m[2][3] = 0;

    result.m[3][0] = d.x;
    result.m[3][1] = d.y;
    result.m[3][2] = d.z;
    result.m[3][3] = 1;

    return result;
}

v3_t mat4x4_transformV3(v3_t in, mat4x4_t mat)
{
    v3_t result;

    result.x = in.x * mat.m[0][0] + in.y * mat.m[1][0] + in.z * mat.m[2][0] + mat.m[3][0];
    result.y = in.x * mat.m[0][1] + in.y * mat.m[1][1] + in.z * mat.m[2][1] + mat.m[3][1];
    result.z = in.x * mat.m[0][2] + in.y * mat.m[1][2] + in.z * mat.m[2][2] + mat.m[3][2];
    float w = in.x * mat.m[0][3] + in.y * mat.m[1][3] + in.z * mat.m[2][3] + mat.m[3][3];

    if (w == 0)
    {
        exit(EXIT_FAILURE);
    }

    result.x /= w;
    result.y /= w;
    result.z /= w;

    return result;
}

mat4x4_t mat4x4_mul(mat4x4_t a, mat4x4_t b)
{
    mat4x4_t result;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            result.m[row][col] = a.m[row][0] * b.m[0][col] + a.m[row][1] * b.m[1][col] + a.m[row][2] * b.m[2][col] + a.m[row][3] * b.m[3][col];
        }
    }

    return result;
}