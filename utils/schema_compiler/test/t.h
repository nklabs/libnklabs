#include <stdbool.h>

#ifndef __SCOMP__
typedef char schar;
#endif

struct fred
{
    int x;
};

struct bob
{
    struct fred a;
    struct { int x; } b;
    bool c;
    schar ident[20];
    schar frak[3][20];
};
