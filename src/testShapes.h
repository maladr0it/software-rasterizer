#ifndef TESTSHAPES_H
#define TESTSHAPES_H

#include "mesh.h"

// clang-format off
#define P_A {-1.0f, -1.0f, -1.0f}
#define P_B {-1.0f, +1.0f, -1.0f}
#define P_C {+1.0f, +1.0f, -1.0f}
#define P_D {+1.0f, -1.0f, -1.0f}
#define P_E {-1.0f, -1.0f, +1.0f}
#define P_F {-1.0f, +1.0f, +1.0f}
#define P_G {+1.0f, +1.0f, +1.0f}
#define P_H {+1.0f, -1.0f, +1.0f}

#define T_A {0.0f, 0.0f}
#define T_B {0.0f, 1.0f}
#define T_C {1.0f, 1.0f}
#define T_D {1.0f, 0.0f}
// clang-format on

tri_t testShapes_cubeTris[] = {
    // front
    {.p = {P_A, P_B, P_D}, .t = {T_A, T_B, T_D}},
    {.p = {P_D, P_B, P_C}, .t = {T_D, T_B, T_C}},

    // right
    {.p = {P_D, P_C, P_H}, .t = {T_A, T_B, T_D}},
    {.p = {P_H, P_C, P_G}, .t = {T_D, T_B, T_C}},

    // back
    {.p = {P_H, P_G, P_E}, .t = {T_A, T_B, T_D}},
    {.p = {P_E, P_G, P_F}, .t = {T_D, T_B, T_C}},

    // left
    {.p = {P_E, P_F, P_A}, .t = {T_A, T_B, T_D}},
    {.p = {P_A, P_F, P_B}, .t = {T_D, T_B, T_C}},

    // top
    {.p = {P_E, P_A, P_H}, .t = {T_A, T_B, T_D}},
    {.p = {P_H, P_A, P_D}, .t = {T_D, T_B, T_C}},

    // bottom
    {.p = {P_B, P_F, P_C}, .t = {T_A, T_B, T_D}},
    {.p = {P_C, P_F, P_G}, .t = {T_D, T_B, T_C}},
};

mesh_t testShapes_cube = {
    .numTris = 12,
    .tris = testShapes_cubeTris,
};

tri_t testShapes_squareTris[] = {
    {.p = {P_A, P_B, P_D}, .t = {T_A, T_B, T_D}},
    {.p = {P_D, P_B, P_C}, .t = {T_D, T_B, T_C}},
};

mesh_t testShapes_square = {
    .numTris = 1,
    .tris = testShapes_squareTris,
};

#endif