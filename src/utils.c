#include "utils.h"

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

void *safeMalloc(size_t size)
{
    void *result = malloc(size);
    if (result == NULL)
    {
        exit(EXIT_FAILURE);
    }
    return result;
}