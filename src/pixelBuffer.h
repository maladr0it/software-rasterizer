#ifndef VIDEOBUFFER_H
#define VIDEOBUFFER_H

#include <stdint.h>

typedef struct pixelBuffer_video
{
    int w;
    int h;
    uint32_t *pixels;
} pixelBuffer_video_t;

typedef struct pixelBuffer_depth
{
    int w;
    int h;
    float *pixels;
} pixelBuffer_depth_t;

pixelBuffer_video_t pixelBuffer_video_create(int w, int h);

pixelBuffer_depth_t pixelBuffer_depth_create(int w, int h);

void pixelBuffer_video_destroy(pixelBuffer_video_t *buffer);

void pixelBuffer_depth_destroy(pixelBuffer_depth_t *buffer);

#endif