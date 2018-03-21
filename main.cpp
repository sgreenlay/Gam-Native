
#include "imports.h"
#include "exports.h"

typedef struct {
    int width;
    int height;
} window;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} character;

static window g_window;
static character g_character;

void init(int w, int h)
{
    g_window.width = w;
    g_window.height = h;

    g_character.x = g_window.width / 2;
    g_character.y = g_window.height / 2;
    g_character.width = 100;
    g_character.height = 100;
}

void mouseMoved(int x, int y)
{
    g_character.x = x;
    g_character.y = y;
}

void render()
{
    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f);
    drawRect(
        g_character.x - (g_character.width / 2),
        g_character.y - (g_character.height / 2),
        g_character.width,
        g_character.height,
        255, 0, 0, 1.0f);
}
