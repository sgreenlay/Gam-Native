
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

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
} box;

static window g_window;
static mouse g_mouse;
static box g_box;

void init(int w, int h)
{
    console_log("hello world");

    g_window.width = w;
    g_window.height = h;

    g_mouse.x = g_window.width / 2;
    g_mouse.y = g_window.height / 2;

    g_box.x = g_window.width / 2;
    g_box.y = g_window.height / 2;
    g_box.vx = -150;
    g_box.vy = 0;
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

void render(int deltaInMs)
{
    memscan();

    g_box.x += (g_box.vx * deltaInMs) / 1000.0f;
    g_box.y += (g_box.vy * deltaInMs) / 1000.0f;

    if (g_box.x < 0)
    {
        g_box.x = -g_box.x;
        g_box.vx *= -1;
    }
    else if (g_box.x > g_window.width)
    {
        g_box.x = g_window.width - (g_box.x - g_window.width);
        g_box.vx *= -1;
    }
    if (g_box.y < 0)
    {
        g_box.y = g_window.height - (g_box.y - g_window.height);
        g_box.vy *= -1;
    }
    else if (g_box.y > g_window.height)
    {
        g_box.y = -g_box.y;
        g_box.vy *= -1;
    }

    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f);

    drawRect(
        g_box.x - 20,
        g_box.y - 20,
        40,
        40,
        0, 0, 255, 1.0f);

    drawRect(
        g_mouse.x - 10,
        g_mouse.y - 10,
        20,
        20,
        255, 0, 0, 1.0f);
}
