#include <stdbool.h>

// Test database
// Type definitions

struct testfwd {
    bool tbool;
    int tint;
    unsigned int tuint;
    char tint8;
    unsigned char tuint8;
    short tint16;
    unsigned short tuint16;
    double tdouble;
    float tfloat;
    schar tstring[20];
};

struct testbkwd {
    schar tstring[20];
    float tfloat;
    double tdouble;
    unsigned short tuint16;
    short tint16;
    unsigned char tuint8;
    char tint8;
    unsigned int tuint;
    int tint;
    bool tbool;
};

struct testtop {
    struct testfwd tstruct;
    struct testbkwd tarray[4];
    //union len tvararray_len;
    //struct testfwd tvararray[8];
    struct testfwd tvararray[3];
    //union len ttable_len;
    //struct testbkwd ttable[7];
    struct testbkwd ttable[3];
};
