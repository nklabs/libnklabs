#include <stdbool.h>

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
