#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include "v3.h"
#include "mat4.h"
#include "tri.h"
#include "mesh.h"
#include "pixelBuffer.h"
#include "texture.h"
#include "utils.h"
#include "console.h"

#include "testShapes.h"

#define DEBUG 0

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;
const int SCENE_WIDTH = 256;
const int SCENE_HEIGHT = 256;
const int BYTES_PER_PX = 4;
const float FOV = 60.0f * M_PI / 180.0f;
const float Z_FAR = 1000.0f;
const float Z_NEAR = 0.1f;

const Uint8 *keyboardState;

mat4_t MAT_PROJ;

int MOUSE_X;
int MOUSE_Y;

uint32_t WHITE;
uint32_t RED;
uint32_t GREEN;
uint32_t BLUE;

pixelBuffer_video_t wallTexture;

uint32_t createColor(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)r << 24) + ((uint32_t)g << 16) + ((uint32_t)b << 8) + 0xff;
};

uint32_t multiplyColor(uint32_t color, float factor)
{
    uint8_t r = (uint8_t)(color >> 24) * factor;
    uint8_t g = (uint8_t)(color >> 16) * factor;
    uint8_t b = (uint8_t)(color >> 8) * factor;
    return createColor(r, g, b);
}

void putPixel(pixelBuffer_video_t *buffer, int x0, int y0, uint32_t color)
{
    buffer->pixels[y0 * buffer->w + x0] = color;
}

void drawRect(pixelBuffer_video_t *buffer, int x0, int y0, int w, int h, uint32_t color)
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

// rasterize tri from screenspace
void rasterizeTri(pixelBuffer_video_t *buffer, pixelBuffer_depth_t *depthBuffer, tri_t projTri, float lightStrength, float p0w, float p1w, float p2w)
{
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
            float w0 = orient2d(projTri.p[0], p, projTri.p[1]) / areax2;
            float w1 = orient2d(projTri.p[1], p, projTri.p[2]) / areax2;
            float w2 = 1 - w0 - w1;

            if (w0 < 0 || w1 < 0 || w2 < 0)
            {
                continue;
            }

            double depth = (w0 * projTri.p[2].z + w1 * projTri.p[0].z + w2 * projTri.p[1].z);
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

            if (depth < depthBuffer->pixels[y * depthBuffer->w + x])
            {
                depthBuffer->pixels[y * depthBuffer->w + x] = depth;

                float w = w0 * (1.0f / p2w) + w1 * (1.0f / p0w) + w2 * (1.0f / p1w);
                float u = w0 * (projTri.t[2].u / p2w) + w1 * (projTri.t[0].u / p0w) + w2 * (projTri.t[1].u / p1w);
                float v = w0 * (projTri.t[2].v / p2w) + w1 * (projTri.t[0].v / p0w) + w2 * (projTri.t[1].v / p1w);

                u /= w;
                v /= w;

                // uint32_t color = texture_get(wallTexture, u, v);
                uint32_t color = WHITE;
                uint32_t shaded = multiplyColor(color, lightStrength);
#if DEBUG
                if (w0 < 0.05 || w1 < 0.05 || w2 < 0.05)
                {
                    shaded = RED;
                }
#endif
                putPixel(buffer, x, y, shaded);
            }
        }
    }
}

void drawTri(pixelBuffer_video_t *buffer, pixelBuffer_depth_t *depthBuffer, tri_t tri, mat4_t transform, mat4_t view)
{
    tri_t transformedTri;
    transformedTri.p[0] = mat4_transformV3(tri.p[0], transform);
    transformedTri.p[1] = mat4_transformV3(tri.p[1], transform);
    transformedTri.p[2] = mat4_transformV3(tri.p[2], transform);
    transformedTri.t[0] = tri.t[0];
    transformedTri.t[1] = tri.t[1];
    transformedTri.t[2] = tri.t[2];

    // lighting
    v3_t normal = tri_getNormal(transformedTri);
    v3_t lightDir = {0, 0, 1};
    lightDir = v3_normalize(lightDir);
    float lightStrength = fMax(-1 * v3_dot(normal, lightDir), 0.0f);
    float globalLight = 0.2f;
    lightStrength = globalLight + lightStrength * (1.0f - globalLight);

    // world space -> view space
    tri_t viewedTri;
    viewedTri.p[0] = mat4_transformV3(transformedTri.p[0], view);
    viewedTri.p[1] = mat4_transformV3(transformedTri.p[1], view);
    viewedTri.p[2] = mat4_transformV3(transformedTri.p[2], view);
    viewedTri.t[0] = transformedTri.t[0];
    viewedTri.t[1] = transformedTri.t[1];
    viewedTri.t[2] = transformedTri.t[2];

    // clip viewed triangle against near plane
    v3_t nearPlanePoint = {0, 0, Z_NEAR};
    v3_t nearPlaneNormal = {0, 0, 1};
    tri_t tris[2];
    int numTris = tri_clipAgainstPlane(tris, nearPlanePoint, nearPlaneNormal, viewedTri);

    for (int i = 0; i < numTris; i++)
    {
        // view space -> screen space

        tri_t tri = tris[i];
        tri_t projTri;

        float p0w;
        projTri.p[0] = mat4_transformV3Proj(&p0w, tri.p[0], MAT_PROJ);
        projTri.p[0] = v3_div(projTri.p[0], p0w);
        projTri.t[0] = tri.t[0];

        float p1w;
        projTri.p[1] = mat4_transformV3Proj(&p1w, tri.p[1], MAT_PROJ);
        projTri.p[1] = v3_div(projTri.p[1], p1w);
        projTri.t[1] = tri.t[1];

        float p2w;
        projTri.p[2] = mat4_transformV3Proj(&p2w, tri.p[2], MAT_PROJ);
        projTri.p[2] = v3_div(projTri.p[2], p2w);
        projTri.t[2] = tri.t[2];

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

        rasterizeTri(buffer, depthBuffer, projTri, lightStrength, p0w, p1w, p2w);
    }
}

void drawMesh(pixelBuffer_video_t *buffer, pixelBuffer_depth_t *depthBuffer, mesh_t mesh, v3_t rot, v3_t trans, mat4_t matView)
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

    pixelBuffer_video_t screen = pixelBuffer_video_create(SCENE_WIDTH, SCENE_HEIGHT);
    pixelBuffer_depth_t depthBuffer = pixelBuffer_depth_create(screen.w, screen.h);

    SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen.w, screen.h);
    if (screenTexture == NULL)
    {
        exit(EXIT_FAILURE);
    }

    wallTexture = texture_load("assets/wall_texture.bmp");

    SDL_Event e;

    MAT_PROJ = mat4_createProj((float)screen.w / (float)screen.h, FOV, Z_NEAR, Z_FAR);

    // mesh_t mesh = testShapes_cube;
    // mesh_t mesh = mesh_load("assets/tri.obj");
    // mesh_t mesh = mesh_load("assets/square.obj");
    // mesh_t mesh = mesh_load("assets/cube.obj");
    mesh_t mesh = mesh_load("assets/teapot.obj");
    // mesh_t mesh = mesh_load("assets/mountains.obj");

    WHITE = createColor(0xff, 0xff, 0xff);
    RED = createColor(0xff, 0x00, 0x00);
    GREEN = createColor(0x00, 0xff, 0x00);
    BLUE = createColor(0x00, 0x00, 0xff);

    bool running = true;
    float cameraYaw = 0.0f;
    v3_t cameraPos = {0, 0, 0};
    v3_t cameraDir = {0, 0, 1};

    float rotX = 0;
    float rotY = 0;
    float rotZ = 0;

    float speed = 0.1f;

    const int numFrameSamples = 60;
    uint32_t lastFrameAt = 0;
    int frameNum = 0;
    float fps = 0;
    uint32_t frameTimes[numFrameSamples];

    while (running)
    {
        // measure perf

        uint32_t now = SDL_GetTicks();
        uint32_t frameTime = now - lastFrameAt;
        frameTimes[frameNum % numFrameSamples] = frameTime;

        if (frameNum % numFrameSamples == numFrameSamples - 1)
        {
            uint32_t sum = 0;
            for (int i = 0; i < numFrameSamples; ++i)
            {
                sum += frameTimes[i];
            }
            fps = 1000.0f / ((float)sum / (float)numFrameSamples);
        }
        lastFrameAt = now;
        frameNum++;

        console_clear();
        console_log("fps: %f", fps);

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
        v3_t upward = v3_mul(cameraUp, speed);
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
            cameraPos = v3_add(cameraPos, upward);
        }
        if (keyboardState[SDL_SCANCODE_DOWN])
        {
            cameraPos = v3_sub(cameraPos, upward);
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

        for (int i = 0; i < depthBuffer.w * depthBuffer.h; i++)
        {
            depthBuffer.pixels[i] = 1.0f;
        }

        // rotX += 0.007;
        rotY += 0.01;
        // rotZ += 0.01;

        v3_t rot = {rotX, rotY, rotZ};
        v3_t trans = {0, 0, 10};
        drawMesh(&screen, &depthBuffer, mesh, rot, trans, matView);

        SDL_UpdateTexture(screenTexture, NULL, screen.pixels, BYTES_PER_PX * screen.w);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        console_render();
        SDL_RenderPresent(renderer);
    }

    return 0;
}
