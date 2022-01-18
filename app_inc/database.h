#ifndef _Idatabase
#define _Idatabase

#include "nkdbase.h"

// Test database

struct testfwd {
    int tbool;
    int tint;
    unsigned int tuint;
    char tint8;
    unsigned char tuint8;
    short tint16;
    unsigned short tuint16;
    double tdouble;
    float tfloat;
    char tstring[10];
};

struct testbkwd {
    char tstring[10];
    float tfloat;
    double tdouble;
    unsigned short tuint16;
    short tint16;
    unsigned char tuint8;
    char tint8;
    unsigned int tuint;
    int tint;
    int tbool;
};

struct testtop {
    struct testfwd tstruct;
    struct testbkwd tarray[2];
    union len tvararray_len;
    struct testfwd tvararray[3];
    union len ttable_len;
    struct testbkwd ttable[3];
};

void database_init();

#endif
