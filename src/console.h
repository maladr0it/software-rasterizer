#ifndef CONSOLE_H
#define CONSOLE_H

#include <SDL2_ttf/SDL_ttf.h>

void console_init(SDL_Renderer *renderer, int line_width);

void console_log(char *format, ...);

void console_clear(void);

void console_render();

void console_destroy(void);

#endif