#ifndef TEXTURE_H
#define TEXTURE_H

#include <SDL2/SDL.h>
#include "pixelBuffer.h"

pixelBuffer_video_t texture_load(char *file);

uint32_t texture_get(pixelBuffer_video_t texture, float u, float v);

void texture_destroy(pixelBuffer_video_t *texture);

#endif