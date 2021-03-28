#ifndef mat4_H
#define mat4_H

#include "v3.h"

typedef struct mat4
{
    float m[4][4];
} mat4_t;

mat4_t mat4_createIdentity();

mat4_t mat4_mul(mat4_t a, mat4_t b);

mat4_t mat4_createProj(float aspectRatio, float fov, float zNear, float zFar);

mat4_t mat4_createRotX(float theta);

mat4_t mat4_createRotY(float theta);

mat4_t mat4_createRotZ(float theta);

mat4_t mat4_createTranslate(v3_t t);

mat4_t mat4_createPointAt(v3_t pos, v3_t target, v3_t up);

mat4_t mat4_createLookAt(mat4_t pointAt);

v3_t mat4_transformV3(v3_t in, mat4_t mat);

v3_t mat4_transformV3Proj(float *w, v3_t in, mat4_t mat);

#endif