#ifndef MESH_H
#define MESH_H

#include "tri.h"

typedef struct mesh
{
    int numTris;
    tri_t *tris;
} mesh_t;

mesh_t mesh_load(char *path);

void mesh_destroy(mesh_t *mesh);

#endif