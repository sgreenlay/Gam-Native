
#include "Imports.h"
#include "Exports.h"

#include "Util.h"

#include "Parser.h"
#include "Plant.h"

extern "C" int atexit(void (*func)()) { return 0; }

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

static plant * p;
static plant * p2;

void init(int w, int h)
{
    console_log("hello world");

    g_window.width = w;
    g_window.height = h;

    p = new plant;
    p2 = new plant;
    *p = plant::make_random({ (double)g_window.width / 2, (double)g_window.height }, - PI / 2);

    g_mouse.x = g_window.width / 2;
    g_mouse.y = g_window.height / 2;
}

void onInput(char * c)
{
    auto rules = parse(c);

    for (auto& rule : rules)
    {
        console_log(rule.start, " ", rule.probability, " ", rule.end, " ", rule.branch);
    }
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

    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f, 0.0f);

    if (g_mouse.click)
    {
        *p = plant::make_random({ (double)g_window.width / 2, (double)g_window.height }, - PI / 2);
    }
    else
    {
        p->physics_into(*p2, { 0, 0.01 }, g_window.height);
        swap(p, p2);
    }
    g_mouse.click = false;

    p->render();

    drawRect(
        g_mouse.x - 10,
        g_mouse.y - 10,
        20,
        20,
        255, 0, 0, 1.0f,
        PI / 4.0f);
}
