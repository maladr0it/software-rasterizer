#include <stdlib.h>
#include "console.h"

typedef struct console
{
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color color;
    int lineWidth;
    int cursorPos;
    char *text;
} console_t;

const int LINE_BUFFER_LEN = 128;
const int TEXT_BUFFER_LEN = 2048;
const SDL_Color COLOR = {255, 255, 255};

static console_t console;
static char console_text[TEXT_BUFFER_LEN];
static TTF_Font *console_font;

void console_init(SDL_Renderer *renderer, int lineWidth)
{
    if (TTF_Init() < 0)
    {
        exit(EXIT_FAILURE);
    }

    console_font = TTF_OpenFont("../assets/NewYork.ttf", 16);
    if (console_font == NULL)
    {
        exit(EXIT_FAILURE);
    }

    // newline is required as console cannot display an empty string due to a SDL_ttf bug
    console_text[0] = '\n';
    console_text[1] = '\0';

    console.renderer = renderer;
    console.font = console_font;
    console.color = COLOR;
    console.lineWidth = lineWidth;
    console.text = console_text;
    console.cursorPos = 1;
}

void console_log(char *format, ...)
{
    va_list vals;
    char buffer[LINE_BUFFER_LEN];

    va_start(vals, format);
    int line_len = vsprintf(buffer, format, vals);
    va_end(vals);

    if (console.cursorPos + line_len >= TEXT_BUFFER_LEN)
    {
        return;
    }

    strcpy(console.text + console.cursorPos, buffer);
    console.cursorPos += line_len;

    // add a blank space to avoid a visual bug with SDL_ttf
    strcpy(console.text + console.cursorPos, " \n");
    console.cursorPos += 2;
}

void console_clear(void)
{
    console.text[0] = '\n';
    console.text[1] = '\0';
    console.cursorPos = 1;
}

// TODO: this could be simpler with a fixed console size, so a direct blit can happen without scaling
void console_render(void)
{
    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(console.font, console.text, console.color, console.lineWidth);
    if (surface == NULL)
    {
        exit(EXIT_FAILURE);
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(console.renderer, surface);
    if (texture == NULL)
    {
        exit(EXIT_FAILURE);
    }

    SDL_Rect destRect = {0, 0, surface->w, surface->h};

    if (SDL_RenderCopy(console.renderer, texture, NULL, &destRect) < 0)
    {
        exit(EXIT_FAILURE);
    };

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void console_destroy(void)
{
    TTF_CloseFont(console.font);
}
