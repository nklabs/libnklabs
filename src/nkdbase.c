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

struct ofilt
{
    const struct nk_dbase *dbase;
    uint32_t pos;
    uint32_t bank_addr;
    unsigned long crc;
};

// Save buffer to flash, calculate CRC along the way

static int dbase_save_block_write(void *ptr, unsigned char *buffer, size_t len)
{
    int sta;
    size_t x;
    struct ofilt *f = (struct ofilt *)ptr;
    unsigned long crc;
    crc = f->crc;
    if (f->pos >= f->dbase->bank_size)
        return -1;
    for (x = 0; x != len; ++x)
        crc = nk_crc32be_update(crc, buffer[x]);
    f->crc = crc;
    sta = f->dbase->flash_write(f->bank_addr + f->pos, buffer, (len + f->dbase->flash_granularity - 1) & ~(f->dbase->flash_granularity - 1));
    f->pos += len;
    return sta;
}

// Database management

int nk_dbase_save(
    const struct nk_dbase *dbase,
    char *rev,
    void *ram
) {
    int x;
    nkoutfile_t f[1];
    int sta = 0;
    struct ofilt ofilt;
    unsigned long crc;
    int bank = (1 & (*rev + 1));

    nk_printf("Saving to bank %d...\n", bank);

    ofilt.dbase = dbase;
    ofilt.pos = 0;
    ofilt.crc = 0;
    if (bank)
    {
        ofilt.bank_addr = dbase->bank1;
    }
    else
    {
        ofilt.bank_addr = dbase->bank0;
    }

    nk_printf("Erasing...\n");
    sta = dbase->flash_erase(ofilt.bank_addr, dbase->bank_size);
    if (sta) {
        nk_printf("  Erase error\n");
        return -1;
    }

    nkoutfile_open(f, dbase_save_block_write, &ofilt, dbase->bigbuf, dbase->bigbuf_size);

    nk_printf("Writing...\n");

    sta |= nk_dbase_serialize(f, dbase->ty, ram);

    sta |= nk_fputc(f, 0); // End of text marker
    sta |= nk_fputc(f, 1 + *rev); // Revision

    crc = ofilt.crc;
    for (x = 0; x != f->ptr - f->start; ++x)
        crc = nk_crc32be_update(crc, f->start[x]);

    sta |= nk_fputc(f, (crc >> 24));
    sta |= nk_fputc(f, (crc >> 16));
    sta |= nk_fputc(f, (crc >> 8));
    sta |= nk_fputc(f, (crc >> 0));

    if (sta) {
        nk_fprintf(nkstderr, "Database is too large! Not saved.\n");
        return -1;
    }

    nk_printf("  size = %u\n", (unsigned int)(ofilt.pos + f->ptr - f->start));
    nk_printf("  rev = %d\n", 1 + *rev);

    nk_fflush(f);

    // Only bump rev if we are successful
    ++*rev;
    nk_printf("done.\n");
    return 0;
}

// Read function for nkinfile: we just call the dbase flash read function, but we limit the length to either the bank size
// (for checking CRC), or to the actual data size (for parsing).

struct filt
{
	const struct nk_dbase *dbase;
	uint32_t bank_addr;
	size_t size;
};

static size_t dbase_flash_read(void *ptr, unsigned char *buffer, size_t offset)
{
	struct filt *f = (struct filt *)ptr;
	if (offset >= f->size)
	{
		return 0;
	}
	else
	{
		f->dbase->flash_read(f->bank_addr + offset, (unsigned char *)f->dbase->bigbuf, f->dbase->bigbuf_size);
		if (f->size - offset >= f->dbase->bigbuf_size)
			return f->dbase->bigbuf_size;
		else
			return f->size - offset;
	}
}

// Check CRC of database of a particular bank.
// Return 0 if it's bad, or the actual data size if it's good

static int nk_dbase_check(int bank, char *rev, const struct nk_dbase *dbase)
{
    unsigned long crc = 0;
    int c;
    nkinfile_t f[1];
    struct filt filt;
    int sta = 0;
    int x;
    nk_printf("Check bank %d:\n", bank);
    filt.dbase = dbase;
    filt.size = dbase->bank_size;
    if (bank)
    {
    	filt.bank_addr = dbase->bank1;
    }
    else
    {
    	filt.bank_addr = dbase->bank0;
    }

    nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    // Fixme: how to handle read errors?
    if (sta) {
        nk_fprintf(nkstderr, "  Read error\n");
        return 0;
    }
    // Find end of text, calculate CRC
    while ((c = nk_fpeek(f)), c != -1 && c)
    {
	crc = nk_crc32be_update(crc, c);
    	nk_fnext_fast(f);
    }

    // NUL
    if (c == -1)  goto badsize;
    crc = nk_crc32be_update(crc, c);
    nk_fnext_fast(f);

    // rev
    c = nk_fpeek(f);
    if (c == -1) goto badsize;
    *rev = c;
    crc = nk_crc32be_update(crc, c);
    nk_fnext_fast(f);

    // CRC bytes
    for (x = 0; x != 4; ++x)
    {
    	c = nk_fpeek(f);
    	if (c == -1) goto badsize;
    	nk_fnext_fast(f);
	crc = nk_crc32be_update(crc, c);
    }

    sta = nk_ftell(f);
    nk_printf("  size = %d\n", sta);
    nk_printf("  rev = %d\n", *rev);
    if (crc)
    {
        nk_fprintf(nkstderr, "  crc is bad\n");
        return 0;
    }

    return sta - 6; // Return size of just the data part

    badsize:
    nk_fprintf(nkstderr, "  size is bad\n");
    return 0;
}

// Choose most recent good version of database and load it

int nk_dbase_load(const struct nk_dbase *dbase, char *rev, void *ram)
{
    nkinfile_t f[1];
    struct filt filt;
    int zero_good = nk_dbase_check(0, rev, dbase);
    char zero_rev = *rev;
    int one_good = nk_dbase_check(1, rev, dbase);
    char one_rev = *rev;
    int use_bank = -1;

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

    filt.dbase = dbase;
    if (use_bank == 0) {
        nk_printf("Using bank 0\n");
        // Need to re-read it
        filt.bank_addr = dbase->bank0;
        filt.size = zero_good;
    	nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    } else if (use_bank == 1) { 
        nk_printf("Using bank 1\n");
        filt.bank_addr = dbase->bank1;
        filt.size = one_good;
    	nkinfile_open(f, dbase_flash_read, &filt, dbase->bigbuf_size, dbase->bigbuf);
    } else {
        nk_fprintf(nkstderr, "Neither bank is good!\n");
        return -1;
    }

    if (nk_fscan(f, " %v", dbase->ty, ram)) {
        nk_printf("Calibration store loaded OK\n");
        return 0;
    } else {
        nk_fprintf(nkstderr, "CRC good, but calibration store failed to parse on load?\n");
        return -1;
    }
}
