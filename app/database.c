// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdbool.h>
#include <string.h>

#include "nkcli.h"
#include "nkprintf.h"
#include "nkscan.h"
#include "nkdbase.h"
#include "nkmcuflash.h"
#include "nkmcuflash_map.h"
#include "nkspiflash.h"
#include "database.h"

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

const struct testtop testdb_defaults = {
    .tstruct = {
        .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.125, .tfloat = .0625, .tstring = "Hello"
    },
    .tarray = {
        { .tstring = "Hello", .tfloat = .0625, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0626, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },

    },
    .tvararray_len.len = 2,
    .tvararray = {
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.126, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.127, .tfloat = .0625, .tstring = "Hello" },
        { .tbool = true, .tint = 0x7FFFFFFE, .tuint = 0xEFFFFFFE, .tint8 = 0x7E, .tuint8 = 0xEE, .tint16 = 0x7FFE, .tuint16 = 0xEFFE, .tdouble = 0.128, .tfloat = .0625, .tstring = "Hello" }
    },
    .ttable_len.len = 2,
    .ttable = {
        { .tstring = "Hello", .tfloat = .0630, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0631, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false },
        { .tstring = "Hello", .tfloat = .0632, .tdouble = 0.125, .tuint16 = 0xEFFE, .tint16 = 0x7FFE, .tuint8 = 0xEE, .tint8 = 0x7E, .tuint = 0xEFFFFFFE, .tint = 0x7FFFFFFE, .tbool = false }
    }
};

struct testtop testdb;
char testdb_rev;

unsigned char xfer_buf[128];

extern const struct nk_spiflash_info m95m04;

const struct nk_dbase test_dbase =
{
    .ty = &tyTESTTOP,

    .bank0 = {
        .area_size = 8192,
        .area_base = 131072,
        .erase_size = 256,
        .info = &m95m04,
        .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_spiflash_read,
        .flash_erase = NULL,
        .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_spiflash_write,
        .granularity = 1
    },

    .bank1 = {
        .area_size = 8192,
        .area_base = 139264,
        .erase_size = 256,
        .info = &m95m04,
        .flash_read = (int (*)(const void *, uint32_t, uint8_t *, uint32_t))nk_spiflash_read,
        .flash_erase = NULL,
        .flash_write = (int (*)(const void *, uint32_t, const uint8_t *, uint32_t))nk_spiflash_write,
        .granularity = 1
    },

    .buf = xfer_buf,
    .buf_size = sizeof(xfer_buf),
    .flash_granularity = 1
};

// Initialize calibration database

void database_init()
{
    int rtn;

    nk_startup_message("Database\n");

    testdb = testdb_defaults;

    rtn = nk_dbase_load(&test_dbase, &testdb_rev, &testdb);
    if (rtn)
    {
        nk_printf("test_db load failed.\n");
    }
}

int cmd_dbase(nkinfile_t *args)
{
    int rtn;
    char key[80];
    if (nk_fscan(args, "clear ")) {
        nk_printf("Clearing calibration store\n");
        testdb = testdb_defaults;
        rtn = nk_dbase_save(&test_dbase, &testdb_rev, &testdb);
        if (rtn)
            nk_printf("Save failure\n");
        rtn = nk_dbase_save(&test_dbase, &testdb_rev, &testdb);
        if (rtn)
            nk_printf("Save failure\n");
        nk_printf("done.\n");
        nk_printf("Please power cycle!\n");
    } else if (nk_fscan(args, "save ")) {
        rtn = nk_dbase_save(&test_dbase, &testdb_rev, &testdb);
        if (rtn)
            nk_printf("Save failure\n");
    } else if (nk_fscan(args, "load ")) {
        rtn = nk_dbase_load(&test_dbase, &testdb_rev, &testdb);
        if (rtn)
            nk_printf("Load failure\n");
    } else if (nk_fscan(args, "show ")) {
        nk_printf("BEGIN\n");
        nk_dbase_fprint(nkstdout, test_dbase.ty, &testdb, 0, "\n");
        nk_printf("END\n");
#if 0
    } else if (nk_fscan(args, "replace ")) {
        char *l = multi_gets();
        cal = cal_defaults;
        if (nk_fscan(&l, " %v ", &tyCAL, &cal)) {
            nk_printf("Replacing keyval store with ");
            print_val(&tyCAL, &cal, 0, "\n");
        } else {
            cal = cal_defaults;
            nk_printf("Syntax error\n");
        }
#endif
    } else if (nk_fscan(args, "get %w ", key, sizeof(key))) {
        void *loc = &testdb;
        uint32_t trigs = 0;
        const struct type *t = nk_xpath(key, &tyTESTTOP, &loc, &trigs);
        if (!t) {
            nk_printf("Not found.\n");
        } else {
            nk_printf("BEGIN\n");
            nk_dbase_fprint(nkstdout, t, loc, 0, "\n");
            nk_printf("END\n");
        }
    } else if (nk_fscan(args, "set %w %e", key, sizeof(key))) {
        void *loc = &testdb;
        uint32_t trigs = 0;
        const struct type *t = nk_xpath(key, &tyTESTTOP, &loc, &trigs);
        if (!t) {
            nk_printf("Not found.\n");
        } else {
            if (nk_fscan(args, "%v ", t, (void *)loc)) {
                nk_printf("Setting %s to ", key);
                nk_dbase_fprint(nkstdout, t, loc, 0, "\n");
#if 0
            } else if (nk_fscan(args, " ")) {
                char *l = multi_gets();
                if (nk_fscan(&l, " %v ", t, (void *)loc)) {
                    nk_printf("Setting %s to ", key);
                    print_val(t, loc, 0, "\n");
                } else {
                    nk_printf("Syntax error\n");
                }
#endif
            } else {
                nk_printf("Syntax error\n");
            }
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_dbase,
    ">dbase                     Database store\n"
    "-dbase clear               Clear database store\n"
    "-dbase show                Show entire database\n"
    "-dbase load                Load database from flash\n"
    "-dbase save                Save database to flash\n"
    "-dbase replace             Replace database: prompts for multi-line value\n"
    "-dbase get <key>           Get value assigned to key\n"
    "-dbase set <key> <value>\n"
    "-                          Assign value to a key\n"
    "-dbase set <key>           Assign value to a key: Prompts for multi-line value\n"
)
