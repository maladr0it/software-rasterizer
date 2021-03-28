#include "texture.h"
#include "console.h"

pixelBuffer_video_t texture_load(char *file)
{
    SDL_Surface *surface = SDL_LoadBMP(file);
    if (surface == NULL)
    {
        exit(EXIT_FAILURE);
    }

    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBX8888);
    SDL_Surface *converted = SDL_ConvertSurface(surface, format, 0);

    pixelBuffer_video_t texture = pixelBuffer_video_create(converted->w, converted->h);
    memcpy(texture.pixels, converted->pixels, converted->w * converted->h * sizeof(Uint32));

    SDL_FreeSurface(surface);
    SDL_FreeSurface(converted);

    return texture;
}

uint32_t texture_get(pixelBuffer_video_t texture, float u, float v)
{
    int x = u * texture.w;
    int y = v * texture.h;
    return texture.pixels[y * texture.w + x];
}

void texture_destroy(pixelBuffer_video_t *texture)
{
    pixelBuffer_video_destroy(texture);
}