
#include "Imports.h"
#include "Exports.h"

#include "Util.h"

typedef struct {
    int width;
    int height;
} window;

typedef struct {
    int x;
    int y;
    bool click;
    bool pressed;
} mouse;

static window g_window;
static mouse g_mouse;

void init(int w, int h)
{
    console_log("hello world");

    g_window.width = w;
    g_window.height = h;

    g_mouse.x = g_window.width / 2;
    g_mouse.y = g_window.height / 2;
}

void mouseMoved(int x, int y, bool pressed)
{
    g_mouse.x = x;
    g_mouse.y = y;

    if (g_mouse.pressed && !pressed)
    {
        g_mouse.click = true;
    }
    g_mouse.pressed = pressed;
}

void render()
{
    memscan();

    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f);

    drawRect(
        g_mouse.x - 10,
        g_mouse.y - 10,
        20,
        20,
        255, 0, 0, 1.0f);
}
