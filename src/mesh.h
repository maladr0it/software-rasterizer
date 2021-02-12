#ifndef MESH_H
#define MESH_H

#include "main.h"

typedef struct mesh
{
    int numTris;
    tri_t *tris;
} mesh_t;

mesh_t loadMesh(char *path);

#endif