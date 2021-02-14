#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "v3.h"
#include "mat4x4.h"
#include "tri.h"
#include "mesh.h"
#include "utils.h"
#include "console.h"

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;
const int BYTES_PER_PX = 4;
const float FOV = 60.0f * M_PI / 180.0f;
const float Z_FAR = 100.0f;
const float Z_NEAR = 0.1f;

#define DEBUG 0

mat4x4_t MAT_PROJ;

int MOUSE_X;
int MOUSE_Y;

Uint32 WHITE;
Uint32 RED;
Uint32 GREEN;
Uint32 BLUE;

typedef struct videoBuffer
{
    int w;
    int h;
    Uint32 *pixels;
} videoBuffer_t;

Uint32 createColor(Uint8 r, Uint8 g, Uint8 b)
{
    return ((Uint32)r << 24) + ((Uint32)g << 16) + ((Uint32)b << 8) + 0xff;
};

Uint32 multiplyColor(Uint32 color, float factor)
{
    Uint8 r = (Uint8)(color >> 24) * factor;
    Uint8 g = (Uint8)(color >> 16) * factor;
    Uint8 b = (Uint8)(color >> 8) * factor;
    return createColor(r, g, b);
}

void putPixel(videoBuffer_t *buffer, int x0, int y0, Uint32 color)
{
    buffer->pixels[y0 * buffer->w + x0] = color;
}

void drawRect(videoBuffer_t *buffer, int x0, int y0, int w, int h, Uint32 color)
{
    int x_start = x0 < 0 ? 0 : x0;
    int x_end = x0 + w > buffer->w ? buffer->w : x0 + w;
    int y_start = y0 < 0 ? 0 : y0;
    int y_end = y0 + h > buffer->h ? buffer->h : y0 + h;

    for (int y = y_start; y < y_end; y++)
    {
        for (int x = x_start; x < x_end; x++)
        {
            putPixel(buffer, x, y, color);
        }
    }
}

// TODO: think of a better name...
float orient2d(v3_t a, v3_t b, v3_t c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void drawTri(videoBuffer_t *buffer, float *depthBuffer, tri_t tri, Uint32 color, mat4x4_t transform)
{
    tri_t transformedTri;
    transformedTri.p[0] = mat4x4_transformV3(tri.p[0], transform);
    transformedTri.p[1] = mat4x4_transformV3(tri.p[1], transform);
    transformedTri.p[2] = mat4x4_transformV3(tri.p[2], transform);

    // lighting
    v3_t normal = tri_getNormal(transformedTri);
    v3_t lightDir = {1, 0, 0};
    lightDir = v3_normalize(lightDir);
    float lightStrength = fMax(-1 * v3_dot(normal, lightDir), 0.0f);
    float globalLight = 0.2f;
    lightStrength = globalLight + lightStrength * (1.0f - globalLight);

    // projection
    tri_t projTri;
    projTri.p[0] = mat4x4_transformV3(transformedTri.p[0], MAT_PROJ);
    projTri.p[1] = mat4x4_transformV3(transformedTri.p[1], MAT_PROJ);
    projTri.p[2] = mat4x4_transformV3(transformedTri.p[2], MAT_PROJ);

    projTri.p[0].y *= -1;
    projTri.p[1].y *= -1;
    projTri.p[2].y *= -1;

    projTri.p[0].x += 1.0f;
    projTri.p[0].y += 1.0f;
    projTri.p[1].x += 1.0f;
    projTri.p[1].y += 1.0f;
    projTri.p[2].x += 1.0f;
    projTri.p[2].y += 1.0f;

    projTri.p[0].x *= 0.5f * (float)buffer->w;
    projTri.p[0].y *= 0.5f * (float)buffer->h;
    projTri.p[1].x *= 0.5f * (float)buffer->w;
    projTri.p[1].y *= 0.5f * (float)buffer->h;
    projTri.p[2].x *= 0.5f * (float)buffer->w;
    projTri.p[2].y *= 0.5f * (float)buffer->h;

    float minX = fMax(fMin3(projTri.p[0].x, projTri.p[1].x, projTri.p[2].x), 0);
    float maxX = fMin(fMax3(projTri.p[0].x, projTri.p[1].x, projTri.p[2].x), buffer->w);
    float minY = fMax(fMin3(projTri.p[0].y, projTri.p[1].y, projTri.p[2].y), 0);
    float maxY = fMin(fMax3(projTri.p[0].y, projTri.p[1].y, projTri.p[2].y), buffer->h);

    float areax2 = orient2d(projTri.p[0], projTri.p[1], projTri.p[2]);

    if (areax2 < 0)
    {
        return;
    }

    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            v3_t p = {x, y, 0};
            float w0 = orient2d(projTri.p[0], projTri.p[1], p);
            float w1 = orient2d(projTri.p[1], projTri.p[2], p);
            float w2 = areax2 - w0 - w1;

            if (w0 < 0 || w1 < 0 || w2 < 0)
            {
                continue;
            }

            double depth = (w0 * projTri.p[2].z + w1 * projTri.p[0].z + w2 * projTri.p[1].z) / areax2;
#if DEBUG
            if (x == MOUSE_X && y == MOUSE_Y)
            {
                console_log("%f, %f, %f", projTri.p[0].z, projTri.p[1].z, projTri.p[2].z);
                console_log("p0: %f, %f %f", projTri.p[0].x, projTri.p[0].y, projTri.p[0].z);
                console_log("p1: %f, %f %f", projTri.p[1].x, projTri.p[1].y, projTri.p[1].z);
                console_log("p2: %f, %f %f", projTri.p[2].x, projTri.p[2].y, projTri.p[2].z);
                console_log("depth: %f", depth);
            }
#endif

            if (depth < depthBuffer[y * buffer->w + x])
            {
                depthBuffer[y * buffer->w + x] = depth;
                Uint32 shaded = multiplyColor(color, lightStrength);
                putPixel(buffer, x, y, shaded);
            }
        }
    }
}

void drawMesh(videoBuffer_t *buffer, float *depthBuffer, mesh_t mesh, v3_t rot, v3_t trans)
{
    mat4x4_t matWorld = mat4x4_createIdentity();
    matWorld = mat4x4_mul(matWorld, mat4x4_createRotX(rot.x));
    matWorld = mat4x4_mul(matWorld, mat4x4_createRotY(rot.y));
    matWorld = mat4x4_mul(matWorld, mat4x4_createRotZ(rot.z));
    matWorld = mat4x4_mul(matWorld, mat4x4_createTranslate(trans));

    for (int i = 0; i < mesh.numTris; i++)
    {
        drawTri(buffer, depthBuffer, mesh.tris[i], WHITE, matWorld);
    }
}

int main(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        exit(EXIT_FAILURE);
    }
    SDL_Window *window = SDL_CreateWindow("Rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        exit(EXIT_FAILURE);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        exit(EXIT_FAILURE);
    }

    console_init(renderer, SCREEN_WIDTH);

    SDL_Event e;

    videoBuffer_t screen;
    screen.w = SCREEN_WIDTH;
    screen.h = SCREEN_HEIGHT;
    screen.pixels = malloc(sizeof(Uint32) * screen.w * screen.h);

    float *depthBuffer = malloc(sizeof(float) * screen.w * screen.h);

    MAT_PROJ = mat4x4_createProj(screen.w / screen.h, FOV, Z_NEAR, Z_FAR);

    SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen.w, screen.h);

    mesh_t mesh = mesh_load("assets/teapot.obj");

    bool running = true;

    WHITE = createColor(0xff, 0xff, 0xff);
    RED = createColor(0xff, 0x00, 0x00);
    GREEN = createColor(0x00, 0xff, 0x00);
    BLUE = createColor(0x00, 0x00, 0xff);

    float rotX = 0;
    float rotY = 0;
    float rotZ = 0;

    while (running)
    {
        console_clear();

        // handle inputs
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        SDL_GetMouseState(&MOUSE_X, &MOUSE_Y);

        rotX += 0.002;
        rotY += 0.010;
        // rotZ += 0.005;

        drawRect(&screen, 0, 0, screen.w, screen.h, createColor(0x00, 0x00, 0x00));

        // reset depth buffer
        for (int i = 0; i < screen.w * screen.h; i++)
        {
            depthBuffer[i] = 1;
        }

        v3_t rot = {rotX, rotY, rotZ};
        v3_t trans = {0, 0, 10};
        drawMesh(&screen, depthBuffer, mesh, rot, trans);

        SDL_UpdateTexture(screenTexture, NULL, screen.pixels, BYTES_PER_PX * screen.w);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        console_render();
        SDL_RenderPresent(renderer);
    }

    return 0;
}