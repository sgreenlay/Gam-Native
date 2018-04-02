
#include "Imports.h"
#include "Exports.h"

#include "Util.h"

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

struct branch
{
    double width;
    double height;

    double x;
    double y;
    double r;

    double dx;
    double dy;
    double dr;

    void render() const
    {
        drawRect(
            x,
            y,
            width,
            height,
            0, 255, 0, 1.0f,
            r);
    }
};

struct constraint
{
    int b1;
    int b2;
    double r;
};

constexpr double PI = 3.141592653;

struct plant
{
    static plant make_random()
    {
        plant ret;

        double x = g_window.width / 2;
        double y = 30;
        double r = PI/2;

        int last = -1;

        int i = 0;
        do
        {
            ret.branches.emplace_back();
            auto& b = ret.branches.back();
            b.width = 25;
            b.height = 5;
            b.x = x;
            b.y = y;
            b.r = r;

            if (last > -1)
                ret.constraints.emplace_back(constraint{last, ret.branches.size() - 1, 0});
            last = ret.branches.size()-1;

            x += 25 * cos(r);
            y += 25 * sin(r);
            r += random() * 0.1;

            if (random() < 0.5)
            {
                make_branch(x, y, r - PI / 2, ret);
            }
            if (random() < 0.5)
            {
                make_branch(x, y, r + PI / 2, ret);
            }
        } while (random() + i*0.05 - 0.2 < 0.95 && i++ != 10);

        return ret;
    }

    static void make_branch(double x, double y, double r, plant& ret)
    {
        int i = 0;
        do
        {
            ret.branches.emplace_back();
            auto& b = ret.branches.back();
            b.width = 15;
            b.height = 2;
            b.x = x;
            b.y = y;
            b.r = r;
            b.dx = 0;
            b.dy = 0;
            b.dr = 0;

            x += 15 * cos(r);
            y += 15 * sin(r);
            r += random() * 0.5;
        } while (random()  + i*0.1 - 0.2 < 0.8 && i++ != 10);
    }

    vector<branch> branches;
    vector<constraint> constraints;

    void physics_into(plant& out) const
    {
        out.branches.resize(branches.size());
        out.constraints.resize(constraints.size());

        for (int x = 0; x < branches.size(); ++x)
        {
            auto& tgt = out.branches[x];
            auto& src = branches[x];
            tgt = src;
            tgt.x += src.dx;
            tgt.y += src.dy;
            tgt.r += src.dr;
            if (tgt.y < 30) tgt.dy = 0;
            else
                tgt.dy -= 0.001;
        }

        for (int y = 0; y < constraints.size(); ++y)
        {
            auto& c = constraints[y];
            out.constraints[y] = c;
            auto& b1src = branches[c.b1];
            auto& b2src = branches[c.b2];
            auto& b1tgt = out.branches[c.b1];
            auto& b2tgt = out.branches[c.b2];
        }
    }

    void render()
    {
        for (auto& b : branches)
        {
            b.render();
        }
    }
};

static plant * p;
static plant * p2;

void init(int w, int h)
{
    log("hello world");

    g_window.width = w;
    g_window.height = h;

    p = new plant;
    p2 = new plant;
    *p = plant::make_random();

    g_mouse.x = g_window.width / 2;
    g_mouse.y = g_window.height / 2;
}

void onInput(char * c)
{
    log(c);
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
    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f, 0.0f);

    if (g_mouse.click)
    {
        *p = plant::make_random();
    }
    else
    {
        p->physics_into(*p2);
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
