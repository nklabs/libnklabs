// Simple flash-based schema-defined database

// Schema of database is defined by 'struct type' and can include
// primitives, strings, structures, variable and fixed-length arrays, and
// tables.

// Serialized version of database with CRC and version number is stored in
// flash.  There are two banks, we ping-pong between them whenever the
// database is saved.  On load, we pick the bank with the highest version
// number that has a valid CRC.

// Serialized form of database looks somewhat like JSON, but not compatible.

// On load, extra fields or missing fields do not interfere with the loading
// process, thereby database migration (changes to schema) is possible.

// Note that heap (malloc) is not used.

// Copyright 2020 NK Labs, LLC

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

#include <stdio.h> // For sprintf
#include <string.h>
#include "nkscan.h"
#include "nkprintf.h"
#include "nkcrclib.h"
#include "nkserialize.h"
#include "nkdbase.h"

// Database management

int nk_dbase_save(
    const struct nk_dbase *dbase,
    char *rev,
    void *ram
) {
    nkoutfile_t f[1];
    int sta = 0;
    nk_checked_t ofilt;
    int bank = (1 & (*rev + 1));

    nk_printf("Saving to bank %d...\n", bank);

    if (bank)
    {
        sta = nk_checked_write_open(&ofilt, &dbase->bank1);
    }
    else
    {
        sta = nk_checked_write_open(&ofilt, &dbase->bank0);
    }

    if (sta) {
        nk_printf("  Erase error\n");
        return -1;
    }

    nkoutfile_open(f, (int (*)(void *,unsigned char *,size_t))nk_checked_write, &ofilt, dbase->buf, dbase->buf_size, ofilt.file->granularity);

    nk_printf("Writing...\n");

    sta |= nk_fputc(f, 1 + *rev); // Revision

    sta |= nk_dbase_serialize(f, dbase->ty, ram);

    sta |= nk_fflush(f);

    sta |= nk_checked_write_close(&ofilt);

    if (sta) {
        nk_fprintf(nkstderr, "  Write error! Not saved.\n");
        return -1;
    }

    nk_printf("  size = %lu\n", ofilt.size);
    nk_printf("  rev = %d\n", 1 + *rev);

    // Only bump rev if we are successful
    ++*rev;
    nk_printf("done.\n");
    return 0;
}

// Choose most recent good version of database and load it

int nk_dbase_load(const struct nk_dbase *dbase, char *rev, void *ram)
{
    nkinfile_t f[1];
    nk_checked_t filt;
    int zero_good;
    char zero_rev;
    int one_good;
    char one_rev;
    int use_bank = -1;

    zero_good = !nk_checked_read_open(&filt, &dbase->bank0, dbase->buf, dbase->buf_size);
    nkinfile_open(f, (size_t (*)(void *,size_t,unsigned char *,size_t))nk_checked_read, &filt, dbase->buf_size, dbase->buf);
    zero_rev = (char)nk_fgetc(f);

    one_good = !nk_checked_read_open(&filt, &dbase->bank1, dbase->buf, dbase->buf_size);
    nkinfile_open(f, (size_t (*)(void *,size_t,unsigned char *,size_t))nk_checked_read, &filt, dbase->buf_size, dbase->buf);
    one_rev = (char)nk_fgetc(f);

    if (one_good && zero_good) {
        if ((signed char)(zero_rev - one_rev) >= 0) // Must be signed comparison equal to rev size!
            use_bank = 0;
        else {
            use_bank = 1;
        }
    } else if (one_good) {
        use_bank = 1;
    } else if (zero_good) {
        use_bank = 0;
    }

    if (use_bank == 0) {
        nk_printf("Using bank 0\n");
        *rev = zero_rev;
        nk_checked_read_open(&filt, &dbase->bank0, dbase->buf, dbase->buf_size);
        nkinfile_open(f, (size_t (*)(void *,size_t,unsigned char *,size_t))nk_checked_read, &filt, dbase->buf_size, dbase->buf);
    } else if (use_bank == 1) { 
        nk_printf("Using bank 1\n");
        *rev = one_rev;
        nk_checked_read_open(&filt, &dbase->bank1, dbase->buf, dbase->buf_size);
        nkinfile_open(f, (size_t (*)(void *,size_t,unsigned char *,size_t))nk_checked_read, &filt, dbase->buf_size, dbase->buf);
    } else {
        nk_fprintf(nkstderr, "Neither bank is good!\n");
        return -1;
    }
    nk_fgetc(f);

    if (nk_fscan(f, " %v", dbase->ty, ram)) {
        nk_printf("Calibration store loaded OK\n");
        return 0;
    } else {
        nk_fprintf(nkstderr, "CRC good, but calibration store failed to parse on load?\n");
        return -1;
    }
}
