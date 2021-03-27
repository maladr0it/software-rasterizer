#include "utils.h"
#include "pixelBuffer.h"

pixelBuffer_video_t pixelBuffer_video_create(int w, int h)
{
    pixelBuffer_video_t buffer;
    buffer.w = w;
    buffer.h = h;
    buffer.pixels = safeMalloc(w * h * sizeof(uint32_t));
    return buffer;
}

pixelBuffer_depth_t pixelBuffer_depth_create(int w, int h)
{
    pixelBuffer_depth_t buffer;
    buffer.w = w;
    buffer.h = h;
    buffer.pixels = safeMalloc(w * h * sizeof(float));
    return buffer;
}

void pixelBuffer_video_destroy(pixelBuffer_video_t *buffer)
{
    free(buffer->pixels);
}

void pixelBuffer_depth_destroy(pixelBuffer_depth_t *buffer)
{
    free(buffer->pixels);
}