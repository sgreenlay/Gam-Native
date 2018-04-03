#if defined(BINARYEN)
#include "Malloc.h"

#include "Imports.h"

constexpr int bytes_per_megabyte = 1024000; // 1 MB
constexpr int max_memory = 2 * bytes_per_megabyte; // 2 MB

static char memory[max_memory];
static int offset = 0;

void* malloc(unsigned int size)
{
    if ((offset + size) > max_memory)
    {
        abort();
    }

    char * buffer = &memory[offset];
    offset += size;
    return buffer;
}

void free(void*)
{
    return;
}

void* operator new[](size_t sz) {
    return malloc(sz);
}

void operator delete[](void* p) {
    return free(p);
}

void* operator new(size_t sz) {
    return malloc(sz);
}

void operator delete(void* p) {
    return free(p);
}
#endif