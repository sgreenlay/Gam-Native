#pragma once

#include <stddef.h>

#include "Exports.h"

#define MEMORYDEBUG 0

void memscan();
void* memset(void*, int, char);
void* memcpy(void*, const void*, unsigned int);

void* malloc(unsigned int);
void free(void*);

void* operator new(size_t sz);
void operator delete(void* p);

void* operator new[](size_t sz);
void operator delete[](void* p);

inline void* operator new(size_t, void* ptr) { return ptr; }
