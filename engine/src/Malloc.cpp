
#include "Malloc.h"

#include "Imports.h"
#include "Util.h"

constexpr int bytes_per_megabyte = 1024000; // 1 MB
constexpr int max_memory = 2 * bytes_per_megabyte;

static char memory[max_memory];
static int end_of_memory = 0;

struct malloc_header {
#if MEMORYDEBUG
    int magic = 0xdeadbeef;
#endif
    int size;
    bool used;
};

void memscan()
{
#if MEMORYDEBUG
    int local_offset = 0;
    while (local_offset < end_of_memory)
    {
        malloc_header * header = (malloc_header *)(memory + local_offset);

        if (header->magic != 0xdeadbeef)
        {
            console_log("corruption ", local_offset);

            abort("Memory corruption");
        }

        if (!header->used)
        {
            char * char_buffer = (char *)header + sizeof(malloc_header);
            for (int i = 0; i < header->size; ++i)
            {
                if (char_buffer[i] != 'd')
                {
                    abort("Use after free");
                }
            }
        }

        local_offset += header->size + sizeof(malloc_header);
    }
#endif
}

void* memset(void * buffer, int size, char v)
{
    char * char_buffer = (char *)buffer;
    for (int i = 0; i < size; ++i)
    {
        char_buffer[i] = v;
    }
    return buffer;
}

void* memcpy(void * dst, const void * src, unsigned int size)
{
    char * char_dst = (char *)dst;
    char * char_src = (char *)src;
    for (int i = 0; i < size; ++i)
    {
        char_dst[i] = char_src[i];
    }
    return dst;
}

void* malloc(const unsigned int size)
{
    const int total_size = size + sizeof(malloc_header);

    int local_offset = 0;
    while (local_offset < end_of_memory)
    {
        malloc_header * header = (malloc_header *)(memory + local_offset);

#if MEMORYDEBUG
        if (header->magic != 0xdeadbeef)
        {
            console_log("corruption ", local_offset);
            abort("Memory corruption");
        }
#endif

        if (!header->used && size <= header->size)
        {
            char * buffer = (char *)header + sizeof(malloc_header);
            
            header->used = true;

#if MEMORYDEBUG
            memset(buffer, header->size, 0);
            console_log("re-use alloc ", (int)((char *)header - memory), " ", header->size, " ", size);
#endif

            return buffer;
        }

        local_offset += header->size + sizeof(malloc_header);
    }

    if ((end_of_memory + total_size) > max_memory)
    {
        abort("Out of memory");
    }

    malloc_header * header = new (memory + end_of_memory) malloc_header;

    header->size = size;
    header->used = true;

    char * buffer = memory + end_of_memory + sizeof(malloc_header);

#if MEMORYDEBUG
    console_log("alloc ", (int)((char *)header - memory), " ", size);
#endif

    end_of_memory += total_size;

    return buffer;
}

void free(void * buffer)
{
    malloc_header * header = (malloc_header *)((char *)buffer - sizeof(malloc_header));

#if MEMORYDEBUG
    if (header->magic != 0xdeadbeef)
    {
        console_log("corruption ", (int)((char *)header - memory));
        abort("Memory corruption");
    }
#endif

    header->used = false;

#if MEMORYDEBUG
    memset(buffer, header->size, 'd');
    console_log("free ", (int)((char *)header - memory), " ", header->size);
#endif
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
