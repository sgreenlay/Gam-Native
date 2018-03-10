extern "C" void log(int n);

extern "C" void drawRect(int x, int y, int w, int h, int r, int g, int b, float a);

typedef struct {
    int width;
    int height;
} window;

static window g_window;

extern "C" void init(int w, int h)
{
    g_window.width = w;
    g_window.height = h;
}

extern "C" void render()
{
    drawRect(0, 0, g_window.width, g_window.height, 255, 255, 255, 1.0f);
    drawRect(
        (g_window.width / 2) - 50,
        (g_window.height / 2) - 50,
        100,
        100,
        255, 0, 0, 1.0f);
}