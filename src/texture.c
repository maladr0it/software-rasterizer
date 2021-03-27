#include "texture.h"

SDL_Surface *texture_create(char *file)
{
    SDL_Surface *surface = SDL_LoadBMP(file);
    if (surface == NULL)
    {
        exit(EXIT_FAILURE);
    }

    SDL_PixelFormat *format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBX8888);
    SDL_Surface *converted = SDL_ConvertSurface(surface, format, 0);
    SDL_FreeSurface(surface);

    return converted;
}

void texture_destroy(SDL_Surface)