#include <stdbool.h>

typedef char _Bool;

struct fred
{
    int x;
};

struct bob
{
    struct fred a;
    struct { int x; } b;
    bool c;
};
