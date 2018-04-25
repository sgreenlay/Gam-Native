
#include "Util.h"

char* strfmt_nz(char* out, int n)
{
    auto b = (char*)(&n+1)-1;
    auto e = b - sizeof(n);

    for (auto c = b; c != e; --c)
    {
        auto c1 = (unsigned char)(*c) % 16;
        auto c2 = (unsigned char)(*c) >> 4;
        *out++ = c2 < 10 ? '0' + c2 : 'a' + c2 - 10;
        *out++ = c1 < 10 ? '0' + c1 : 'a' + c1 - 10;
    }
    return out;
}

char* strfmt_nz(char* out, const char* s)
{
    while (*s != 0) *out++ = *s++;
    return out;
}
