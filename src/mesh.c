#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mesh.h"

const int MAX_LINE_LEN = 64;
const int MAX_VERTS = 8192;
const int MAX_TRIS = 8192;

mesh_t mesh_load(char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        exit(EXIT_FAILURE);
    }

    mesh_t mesh;
    mesh.tris = malloc(sizeof(tri_t) * MAX_TRIS);
    mesh.numTris = 0;
    v3_t *verts = malloc(sizeof(v3_t) * MAX_VERTS);

    int numVerts = 0;
    char line[MAX_LINE_LEN];

    while (fgets(line, MAX_LINE_LEN, file) != NULL)
    {
        char *p;

        switch (line[0])
        {
        case 'v':
            p = line + 1;
            v3_t vert;

            vert.x = strtof(p, &p);
            vert.y = strtof(p, &p);
            vert.z = strtof(p, &p);

            verts[numVerts] = vert;
            numVerts++;
            break;

        case 'f':
            p = line + 1;
            tri_t tri;

            int vertAIdx = strtol(p, &p, 10) - 1;
            tri.p[0] = verts[vertAIdx];

            int vertBIdx = strtol(p, &p, 10) - 1;
            tri.p[1] = verts[vertBIdx];

            int vertCIdx = strtol(p, &p, 10) - 1;
            tri.p[2] = verts[vertCIdx];

            mesh.tris[mesh.numTris] = tri;
            mesh.numTris++;
            break;
        }
    }
    free(verts);
    return mesh;
}

void mesh_unload(mesh_t *mesh)
{
    free(mesh->tris);
}