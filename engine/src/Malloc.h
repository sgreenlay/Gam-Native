
#include <stddef.h>

void* malloc(unsigned int);
void free(void*);

void* operator new(size_t sz);
void operator delete(void* p);

void* operator new[](size_t sz);
void operator delete[](void* p);

inline void* operator new(size_t, void* ptr) { return ptr; }
