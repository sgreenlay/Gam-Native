
extern "C" void* malloc(unsigned int);
extern "C" void free(void*);
extern "C" void* memcpy(void*, const void*, unsigned int);
extern "C" void* memset(void*, int, char);

extern "C" void onInput(char *);

extern "C" void init(int w, int h);
extern "C" void render();

extern "C" void mouseMoved(int x, int y, bool pressed);
