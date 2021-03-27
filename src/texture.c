#include "texture.h"

pixelBuffer_video_t texture_create(char *file)
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

void texture_destroy(pixelBuffer_video_t *texture)
{
    pixelBuffer_video_destroy(texture);
}