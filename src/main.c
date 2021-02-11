#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>

const int SCREEN_WIDTH = 512;
const int SCREEN_HEIGHT = 512;
const float FOV = 60.0f * M_PI / 180.0f;
const float Z_FAR = 1000.0f;
const float Z_NEAR = 0.1f;

float MAT_PROJ[4][4];

typedef struct videoBuffer
{
    int w;
    int h;
    Uint32 *pixels;
} videoBuffer_t;

typedef struct v3
{
    float x;
    float y;
    float z;
} v3_t;

typedef struct tri
{
    v3_t p[3];
} tri_t;

typedef float mat4x4_t[4][4];

Uint32 makeColor(Uint8 r, Uint8 g, Uint8 b)
{
    return ((Uint32)r << 24) + ((Uint32)g << 16) + ((Uint32)b << 8) + 0xff;
};

float fMin(float a, float b)
{
    return a < b ? a : b;
}

float fMax(float a, float b)
{
    return a > b ? a : b;
}

float fMin3(float a, float b, float c)
{
    if (a < b)
    {
        if (c < a)
        {
            return c;
        }
        else
        {
            return a;
        }
    }
    else
    {
        if (b < c)
        {
            return b;
        }
        else
        {
            return c;
        }
    }
}

float fMax3(float a, float b, float c)
{
    if (a > b)
    {
        if (c > a)
        {
            return c;
        }
        else
        {
            return a;
        }
    }
    else
    {
        if (b > c)
        {
            return b;
        }
        else
        {
            return c;
        }
    }
}

void createMatProj(mat4x4_t out)
{
    float a = SCREEN_WIDTH / SCREEN_HEIGHT;
    float f = 1 / tan(FOV / 2);
    float q = Z_FAR / (Z_FAR - Z_NEAR);

    out[0][0] = a * f;
    out[0][1] = 0;
    out[0][2] = 0;
    out[0][3] = 0;

    out[1][0] = 0;
    out[1][1] = f;
    out[1][2] = 0;
    out[1][3] = 0;

    out[2][0] = 0;
    out[2][1] = 0;
    out[2][2] = q;
    out[2][3] = 1;

    out[3][0] = 0;
    out[3][1] = 0;
    out[3][2] = -Z_NEAR * q;
    out[3][3] = 0;
}

void createMatRotX(mat4x4_t out, float theta)
{
    out[0][0] = 1;
    out[0][1] = 0;
    out[0][2] = 0;
    out[0][3] = 0;

    out[1][0] = 0;
    out[1][1] = cosf(theta);
    out[1][2] = sinf(theta);
    out[1][3] = 0;

    out[2][0] = 0;
    out[2][1] = -sinf(theta);
    out[2][2] = cosf(theta);
    out[2][3] = 0;

    out[3][0] = 0;
    out[3][1] = 0;
    out[3][2] = 0;
    out[3][3] = 1;
}

void createMatRotZ(mat4x4_t out, float theta)
{
    out[0][0] = cosf(theta);
    out[0][1] = sinf(theta);
    out[0][2] = 0;
    out[0][3] = 0;

    out[1][0] = -sinf(theta);
    out[1][1] = cosf(theta);
    out[1][2] = 0;
    out[1][3] = 0;

    out[2][0] = 0;
    out[2][1] = 0;
    out[2][2] = 1;
    out[2][3] = 0;

    out[3][0] = 0;
    out[3][1] = 0;
    out[3][2] = 0;
    out[3][3] = 1;
}

void createMatTrans(mat4x4_t out, v3_t d)
{
    out[0][0] = 1;
    out[0][1] = 0;
    out[0][2] = 0;
    out[0][3] = 0;

    out[1][0] = 0;
    out[1][1] = 1;
    out[1][2] = 0;
    out[1][3] = 0;

    out[2][0] = 0;
    out[2][1] = 0;
    out[2][2] = 1;
    out[2][3] = 0;

    out[3][0] = d.x;
    out[3][1] = d.y;
    out[3][2] = d.z;
    out[3][3] = 1;
}

void putPixel(videoBuffer_t *buffer, int x0, int y0, Uint32 color)
{
    if (x0 >= 0 && x0 < buffer->w && y0 >= 0 && y0 < buffer->h)
    {
        buffer->pixels[(buffer->h - 1 - y0) * buffer->w + x0] = color;
    }
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

float orient2d(v3_t a, v3_t b, v3_t c)
{
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void transformVector(v3_t *out, v3_t in, mat4x4_t mat)
{
    out->x = in.x * mat[0][0] + in.y * mat[1][0] + in.z * mat[2][0] + mat[3][0];
    out->y = in.x * mat[0][1] + in.y * mat[1][1] + in.z * mat[2][1] + mat[3][1];
    out->z = in.x * mat[0][2] + in.y * mat[1][2] + in.z * mat[2][2] + mat[3][2];
    float w = in.x * mat[0][3] + in.y * mat[1][3] + in.z * mat[2][3] + mat[3][3];

    if (w != 0.0f)
    {
        out->x /= w;
        out->y /= w;
        out->z /= w;
    }
}

void drawTri(videoBuffer_t *buffer, tri_t tri, Uint32 color, float rotX)
{
    tri_t rotXTri;
    tri_t rotZTri;
    tri_t transTri;
    tri_t projTri;

    mat4x4_t matRotX;
    mat4x4_t matRotZ;
    mat4x4_t matTrans;

    createMatRotX(matRotX, 0);
    createMatRotZ(matRotZ, rotX);
    v3_t d = {.x = 0, .y = 0, .z = 100};
    createMatTrans(matTrans, d);

    transformVector(&(rotXTri.p[0]), tri.p[0], matRotX);
    transformVector(&(rotXTri.p[1]), tri.p[1], matRotX);
    transformVector(&(rotXTri.p[2]), tri.p[2], matRotX);

    transformVector(&(rotZTri.p[0]), rotXTri.p[0], matRotZ);
    transformVector(&(rotZTri.p[1]), rotXTri.p[1], matRotZ);
    transformVector(&(rotZTri.p[2]), rotXTri.p[2], matRotZ);

    transformVector(&(transTri.p[0]), rotZTri.p[0], matTrans);
    transformVector(&(transTri.p[1]), rotZTri.p[1], matTrans);
    transformVector(&(transTri.p[2]), rotZTri.p[2], matTrans);

    transformVector(&(projTri.p[0]), transTri.p[0], MAT_PROJ);
    transformVector(&(projTri.p[1]), transTri.p[1], MAT_PROJ);
    transformVector(&(projTri.p[2]), transTri.p[2], MAT_PROJ);

    projTri.p[0].x += 1.0f;
    projTri.p[0].y += 1.0f;
    projTri.p[1].x += 1.0f;
    projTri.p[1].y += 1.0f;
    projTri.p[2].x += 1.0f;
    projTri.p[2].y += 1.0f;

    projTri.p[0].x *= 0.5 * (float)buffer->w;
    projTri.p[0].y *= 0.5 * (float)buffer->h;
    projTri.p[1].x *= 0.5 * (float)buffer->w;
    projTri.p[1].y *= 0.5 * (float)buffer->h;
    projTri.p[2].x *= 0.5 * (float)buffer->w;
    projTri.p[2].y *= 0.5 * (float)buffer->h;

    float minX = fMax(fMin3(projTri.p[0].x, projTri.p[1].x, projTri.p[2].x), 0);
    float maxX = fMin(fMax3(projTri.p[0].x, projTri.p[1].x, projTri.p[2].x), buffer->h);
    float minY = fMax(fMin3(projTri.p[0].y, projTri.p[1].y, projTri.p[2].y), 0);
    float maxY = fMin(fMax3(projTri.p[0].y, projTri.p[1].y, projTri.p[2].y), buffer->w);

    for (int y = minY; y < maxY; y++)
    {
        for (int x = minX; x < maxX; x++)
        {
            v3_t p = {x, y, 0};
            float w0 = orient2d(projTri.p[0], projTri.p[1], p);
            float w1 = orient2d(projTri.p[1], projTri.p[2], p);
            float w2 = orient2d(projTri.p[2], projTri.p[0], p);

            if (w0 >= 0 && w1 >= 0 && w2 >= 0)
            {
                putPixel(buffer, x, y, color);
            }
        }
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

    SDL_Event e;

    videoBuffer_t screen;
    screen.w = SCREEN_WIDTH;
    screen.h = SCREEN_HEIGHT;
    Uint32 screenPixels[screen.w * screen.h];
    screen.pixels = screenPixels;

    SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    bool running = true;

    createMatProj(MAT_PROJ);

    Uint32 red = makeColor(0xff, 0x00, 0x00);
    Uint32 green = makeColor(0x00, 0xff, 0x00);

    tri_t triA = {
        .p = {
            {-10.0f, 0.0f, 0.0f},
            {10.0f, 2.0f, 0.0f},
            {0.0f, 10.0f, 0.0f}}};

    float rotX = 0;

    while (running)
    {
        // handle inputs
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
            {
                running = false;
            }
        }

        rotX += 0.01;

        // render
        drawRect(&screen, 0, 0, screen.w, screen.h, makeColor(0x00, 0x00, 0x00));
        drawTri(&screen, triA, green, rotX);
        SDL_UpdateTexture(screenTexture, NULL, screen.pixels, 4 * screen.w);
        SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    return 0;
}