#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "v3.h"
#include "mat4.h"
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

const Uint8 *keyboardState;

#define DEBUG 0

mat4_t MAT_PROJ;

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

void drawTri(videoBuffer_t *buffer, float *depthBuffer, tri_t tri, mat4_t transform, mat4_t view)
{
    Uint32 color = WHITE;

    tri_t transformedTri, projTri, viewedTri;

    transformedTri.p[0] = mat4_transformV3(tri.p[0], transform);
    transformedTri.p[1] = mat4_transformV3(tri.p[1], transform);
    transformedTri.p[2] = mat4_transformV3(tri.p[2], transform);

    // lighting
    v3_t normal = tri_getNormal(transformedTri);
    v3_t lightDir = {1, 0, 0};
    lightDir = v3_normalize(lightDir);
    float lightStrength = fMax(-1 * v3_dot(normal, lightDir), 0.0f);
    float globalLight = 0.2f;
    lightStrength = globalLight + lightStrength * (1.0f - globalLight);

    // world space -> view space
    viewedTri.p[0] = mat4_transformV3(transformedTri.p[0], view);
    viewedTri.p[1] = mat4_transformV3(transformedTri.p[1], view);
    viewedTri.p[2] = mat4_transformV3(transformedTri.p[2], view);

    // viewedTri = transformedTri;

    // view space -> screen space
    projTri.p[0] = mat4_transformV3(viewedTri.p[0], MAT_PROJ);
    projTri.p[1] = mat4_transformV3(viewedTri.p[1], MAT_PROJ);
    projTri.p[2] = mat4_transformV3(viewedTri.p[2], MAT_PROJ);

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

    float areax2 = orient2d(projTri.p[2], projTri.p[1], projTri.p[0]);

    if (areax2 < 0)
    {
        return;
    }

    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            v3_t p = {x, y, 0};
            float w0 = orient2d(projTri.p[0], p, projTri.p[1]);
            float w1 = orient2d(projTri.p[1], p, projTri.p[2]);
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

void drawMesh(videoBuffer_t *buffer, float *depthBuffer, mesh_t mesh, v3_t rot, v3_t trans, mat4_t matView)
{
    mat4_t matWorld = mat4_createIdentity();
    matWorld = mat4_mul(matWorld, mat4_createRotX(rot.x));
    matWorld = mat4_mul(matWorld, mat4_createRotY(rot.y));
    matWorld = mat4_mul(matWorld, mat4_createRotZ(rot.z));
    matWorld = mat4_mul(matWorld, mat4_createTranslate(trans));

    v3_t up = {0, 1, 0};

    for (int i = 0; i < mesh.numTris; i++)
    {
        drawTri(buffer, depthBuffer, mesh.tris[i], matWorld, matView);
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

    MAT_PROJ = mat4_createProj((float)screen.w / (float)screen.h, FOV, Z_NEAR, Z_FAR);

    SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen.w, screen.h);

    mesh_t mesh = mesh_load("assets/teapot2.obj");

    WHITE = createColor(0xff, 0xff, 0xff);
    RED = createColor(0xff, 0x00, 0x00);
    GREEN = createColor(0x00, 0xff, 0x00);
    BLUE = createColor(0x00, 0x00, 0xff);

    bool running = true;
    float cameraYaw = 0.0f;
    v3_t cameraPos = {0, 0, 0};
    v3_t cameraDir = {0, 0, 1};

    // m4_t transformRotY = m4_createRotY(cameraYaw);

    // cameraDir = v3_normalize(cameraDir);

    float rotX = 0;
    float rotY = 0;
    float rotZ = 0;

    float speed = 0.5f;

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

        v3_t cameraUp = {0, -1, 0};
        v3_t cameraTarget = {0, 0, 1};
        v3_t forward = v3_mul(cameraDir, speed);
        v3_t strafe = v3_cross(forward, cameraUp);

        keyboardState = SDL_GetKeyboardState(NULL);

        if (keyboardState[SDL_SCANCODE_W])
        {
            cameraPos = v3_add(cameraPos, forward);
        }
        if (keyboardState[SDL_SCANCODE_S])
        {
            cameraPos = v3_sub(cameraPos, forward);
        }
        if (keyboardState[SDL_SCANCODE_A])
        {
            cameraPos = v3_sub(cameraPos, strafe);
        }
        if (keyboardState[SDL_SCANCODE_D])
        {
            cameraPos = v3_add(cameraPos, strafe);
        }
        if (keyboardState[SDL_SCANCODE_UP])
        {
            cameraPos = v3_add(cameraPos, cameraUp);
        }
        if (keyboardState[SDL_SCANCODE_DOWN])
        {
            cameraPos = v3_sub(cameraPos, cameraUp);
        }
        if (keyboardState[SDL_SCANCODE_LEFT])
        {
            cameraYaw -= 0.05f;
        }
        if (keyboardState[SDL_SCANCODE_RIGHT])
        {
            cameraYaw += 0.05f;
        }

        mat4_t matCameraRot = mat4_createRotY(cameraYaw);
        cameraDir = mat4_transformV3(cameraTarget, matCameraRot);
        mat4_t matCamera = mat4_createPointAt(cameraPos, v3_add(cameraPos, cameraDir), cameraUp);
        mat4_t matView = mat4_createLookAt(matCamera);

        // render
        drawRect(&screen, 0, 0, screen.w, screen.h, createColor(0x00, 0x00, 0x00));

        // reset depth buffer
        for (int i = 0; i < screen.w * screen.h; i++)
        {
            depthBuffer[i] = 1;
        }

        // draw mesh

        rotX += 0.01;
        rotY += 0.01;
        // rotZ += 0.01;

        v3_t rot = {rotX, rotY, rotZ};
        v3_t trans = {0, 0, 30};
        drawMesh(&screen, depthBuffer, mesh, rot, trans, matView);

        SDL_UpdateTexture(screenTexture, NULL, screen.pixels, BYTES_PER_PX * screen.w);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        console_render();
        SDL_RenderPresent(renderer);
    }

    return 0;
}