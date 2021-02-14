#ifndef MAT4X4_H
#define MAT4X4_H

#include "v3.h"

typedef struct mat4x4
{
    float m[4][4];
} mat4x4_t;

mat4x4_t mat4x4_createProj(float aspectRatio, float fov, float zNear, float zFar);

mat4x4_t mat4x4_createRotX(float theta);

mat4x4_t mat4x4_createRotY(float theta);

mat4x4_t mat4x4_createRotZ(float theta);

mat4x4_t mat4x4_createTranslate(v3_t d);

v3_t mat4x4_transformV3(v3_t in, mat4x4_t mat);

#endif