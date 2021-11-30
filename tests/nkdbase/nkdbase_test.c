#include <stdbool.h>
#include <string.h>

#include "nkprintf.h"
#include "nkscan.h"
#include "nkdbase.h"

// Test database
// Type definitions

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
    char tstring[20];
};

struct testbkwd {
    char tstring[20];
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
    struct testbkwd tarray[4];
    union len tvararray_len;
    struct testfwd tvararray[8];
    union len ttable_len;
    struct testbkwd ttable[7];
};

// Test database metadata

const struct type tyTESTSTRING = {
    .what = tSTRING,
    .size = nk_member_size(struct testfwd, tstring),
    .members = NULL,
    .subtype = NULL,
    .check = NULL
};

const struct member testfwd_members[] = {
    { "tbool", &tyBOOL, offsetof(struct testfwd, tbool) },
    { "tint", &tyINT, offsetof(struct testfwd, tint) },
    { "tuint", &tyUINT, offsetof(struct testfwd, tuint) },
    { "tint8", &tyINT8, offsetof(struct testfwd, tint8) },
    { "tuint8", &tyUINT8, offsetof(struct testfwd, tuint8) },
    { "tint16", &tyINT16, offsetof(struct testfwd, tint16) },
    { "tuint16", &tyUINT16, offsetof(struct testfwd, tuint16) },
    { "tdouble", &tyDOUBLE, offsetof(struct testfwd, tdouble) },
    { "tfloat", &tyFLOAT, offsetof(struct testfwd, tfloat) },
    { "tstring", &tyTESTSTRING, offsetof(struct testfwd, tstring) },
    { NULL, NULL, 0 }
};

const struct type tyTESTFWD = {
    .what = tSTRUCT,
    .size = sizeof(struct testfwd),
    .members = testfwd_members,
    .subtype = NULL,
    .check = NULL
};

const struct member testbkwd_members[] = {
    { "tstring", &tyTESTSTRING, offsetof(struct testbkwd, tstring) },
    { "tfloat", &tyFLOAT, offsetof(struct testbkwd, tfloat) },
    { "tdouble", &tyDOUBLE, offsetof(struct testbkwd, tdouble) },
    { "tuint16", &tyUINT16, offsetof(struct testbkwd, tuint16) },
    { "tint16", &tyINT16, offsetof(struct testbkwd, tint16) },
    { "tuint8", &tyUINT8, offsetof(struct testbkwd, tuint8) },
    { "tint8", &tyINT8, offsetof(struct testbkwd, tint8) },
    { "tuint", &tyUINT, offsetof(struct testbkwd, tuint) },
    { "tint", &tyINT, offsetof(struct testbkwd, tint) },
    { "tbool", &tyBOOL, offsetof(struct testbkwd, tbool) },
    { NULL, NULL, 0 }
};

const struct type tyTESTBKWD = {
    .what = tSTRUCT,
    .size = sizeof(struct testbkwd),
    .members = testbkwd_members,
    .subtype = NULL,
    .check = NULL
};

const struct type tyTESTARRAY = {
    .what = tARRAY,
    .size = nk_member_size(struct testtop, tarray),
    .members = NULL,
    .subtype = &tyTESTBKWD,
    .check = NULL
};

const struct type tyTESTVARARRAY = {
    .what = tVARRAY,
    .size = nk_member_size(struct testtop, tvararray),
    .members = NULL,
    .subtype = &tyTESTFWD,
    .check = NULL
};

const struct type tyTESTTABLE = {
    .what = tTABLE,
    .size = nk_member_size(struct testtop, ttable),
    .members = NULL,
    .subtype = &tyTESTBKWD,
    .check = NULL
};

const struct member testtop_members[] = {
    { "tstruct", &tyTESTFWD, offsetof(struct testtop, tstruct) },
    { "tarray", &tyTESTARRAY, offsetof(struct testtop, tarray) },
    { "tvararray", &tyTESTVARARRAY, offsetof(struct testtop, tvararray_len) },
    { "ttable", &tyTESTTABLE, offsetof(struct testtop, ttable_len) },
    { NULL, NULL, 0 }
};

const struct type tyTESTTOP = {
    .what = tSTRUCT,
    .size = sizeof(struct testtop),
    .members = testtop_members,
    .subtype = NULL,
    .check = NULL
};

// Example loaded structure

struct testtop testtop = {
    .tstruct = {
        .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.125, .tfloat = .0625, .tstring = "Hello"
    },
    .tarray = {
        { .tstring = "Hello", .tfloat = .0625, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0626, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0627, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0628, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },

    },
    .tvararray_len.len = 3,
    .tvararray = {
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.126, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.127, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.128, .tfloat = .0625, .tstring = "Hello" }
    },
    .ttable_len.len = 3,
    .ttable = {
        { .tstring = "Hello", .tfloat = .0630, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0631, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0632, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false }
    }
};

struct testtop tryit;

int main(int argc, char *argv[])
{
    // Serialized format
    nk_dbase_serialize(nkstdout, &tyTESTTOP, &testtop);
    nk_printf("\n");

    // Printable format
    nk_dbase_fprint(nkstdout, &tyTESTTOP, &testtop, 0, "\n");

    char *ser = "{tstruct:{tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.125, tfloat:0.0625, tstring:\"Hello\"}, tarray:[{tstring:\"Hello\", tfloat:0.0625, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0626, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0627, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0628, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}], tvararray:[{tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.126, tfloat:0.0625, tstring:\"Hello\"}, {tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.127, tfloat:0.0625, tstring:\"Hello\"}, {tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.128, tfloat:0.0625, tstring:\"Hello\"}], ttable:(tstring tfloat tdouble tuint16 tint16 tuint8 tint8 tuint tint tbool :\"Hello\" 0.063 0.125 61438 32766 238 126 4026531838 2147483646 false:\"Hello\" 0.0631 0.125 61438 32766 238 126 4026531838 2147483646 false:\"Hello\" 0.0632 0.125 61438 32766 238 126 4026531838 2147483646 false)}";
    nkinfile_t f[1];
    nkinfile_open_string(f, ser);
    int sta;
    sta = nk_fscan(f, "%v", &tyTESTTOP, &tryit);
    nk_printf("Parse status = %d\n", sta);
    nk_dbase_fprint(nkstdout, &tyTESTTOP, &tryit, 0, "\n");
    if (memcmp(&tryit, &testtop, sizeof(struct testtop)))
        printf("Mismatch!\n");
    else
        printf("They match!\n");
}
