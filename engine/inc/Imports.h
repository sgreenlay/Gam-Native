
#if defined(BINARYEN)
extern "C" void abort();

extern "C" double cos(double radians);
extern "C" double sin(double radians);

#else
#include <stdlib.h>
#include <math.h>
#endif

extern "C" void console_log(char*);

extern "C" double random();
extern "C" void drawRect(int x, int y, int w, int h, int r, int g, int b, float a, double radians);
