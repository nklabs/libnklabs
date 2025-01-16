#include <stdbool.h>
#include <string.h>

#include "nkprintf.h"
#include "nkscan.h"
#include "nkdbase.h"

#include "database.h"

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
    //.tvararray_len.len = 3,
    .tvararray = {
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.126, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.127, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.128, .tfloat = .0625, .tstring = "Hello" }
    },
    //.ttable_len.len = 3,
    .ttable = {
        { .tstring = "Hello", .tfloat = .0630, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0631, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0632, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false }
    }
};

struct testtop tryit;

extern struct type ty_testtop;

int main(int argc, char *argv[])
{
    // Serialized format
    nk_dbase_serialize(nkstdout, &ty_testtop, &testtop);
    nk_printf("\n");

    // Printable format
    nk_dbase_fprint(nkstdout, &ty_testtop, &testtop, 0, "\n");

    char *ser = "{tstruct:{tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.125, tfloat:0.0625, tstring:\"Hello\"}, tarray:[{tstring:\"Hello\", tfloat:0.0625, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0626, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0627, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}, {tstring:\"Hello\", tfloat:0.0628, tdouble:0.125, tuint16:61438, tint16:32766, tuint8:238, tint8:126, tuint:4026531838, tint:2147483646, tbool:false}], tvararray:[{tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.126, tfloat:0.0625, tstring:\"Hello\"}, {tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.127, tfloat:0.0625, tstring:\"Hello\"}, {tbool:true, tint:2147483646, tuint:4026531838, tint8:126, tuint8:238, tint16:32766, tuint16:61438, tdouble:0.128, tfloat:0.0625, tstring:\"Hello\"}], ttable:(tstring tfloat tdouble tuint16 tint16 tuint8 tint8 tuint tint tbool :\"Hello\" 0.063 0.125 61438 32766 238 126 4026531838 2147483646 false:\"Hello\" 0.0631 0.125 61438 32766 238 126 4026531838 2147483646 false:\"Hello\" 0.0632 0.125 61438 32766 238 126 4026531838 2147483646 false)}";
    nkinfile_t f[1];
    nkinfile_open_string(f, ser);
    int sta;
    sta = nk_fscan(f, "%v", &ty_testtop, &tryit);
    nk_printf("Parse status = %d\n", sta);
    nk_dbase_fprint(nkstdout, &ty_testtop, &tryit, 0, "\n");
    if (memcmp(&tryit, &testtop, sizeof(struct testtop)))
        printf("Mismatch!\n");
    else
        printf("They match!\n");
}
