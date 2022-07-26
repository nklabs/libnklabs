// File access abstraction

//  Access a string as a file
//  Access a memory block as a file
//  Access a block device as a file

// Supports random access through tell/seek

// This was written for nkscan and nkdbase, so that we can parse a database
// located in SPI-flash without first having to copy the string to a large
// RAM buffer.  Instead all of the parsing functions read the input as if
// it was a file, where the device level access is provided via block reads
// to a small buffer.

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

#ifndef _nkinfile
#define _nkinfile

#include <stdlib.h> // For ssize_t

#include "nkoutfile.h"

// Nkfile pointer

typedef struct nkinfile nkinfile_t;

struct nkinfile
{
    int tmp; // Temporary character
    size_t tmp_pos; // Temporary position for nkfpeek_rel and nkfpeek_abs

    // Information about current buffer
    unsigned char *ptr; // Current pointer into current buffer
    unsigned char *start; // Start of current buffer
    unsigned char *end; // End of current buffer
    size_t len; // Number of bytes available in current buffer
    size_t start_offset; // File offset of first byte of current buffer

    // For abstracting some kind of fixed-length block-based storage system into a file
    void *block_read_ptr; // Pointer to pass to block_read
    size_t (*block_read)(void *block_read_ptr, size_t pos, unsigned char *buffer, size_t block_size);
    size_t block_size; // Block size, also buffer must be this size
};

// Return current file position
// Return 0..file_size.  If it returns file_size, it means we are at end of file.
#define nk_ftell(f) ((size_t)((f)->start_offset + (f)->ptr - (f)->start))

// True if we're at end of file
#define nk_feof(f) ((f)->ptr == (f)->end)


// Set current file position and return character there
// pos must be 0..file_size, inclusive.  It's allowed to set file position to the end of the file, in which case -1 is returned
int nk_fseek_slow(nkinfile_t *f, size_t pos);

// Set current file position and return character there
// Target position must be within current buffer
#define nk_fseek_fast(f, n) (*((f)->ptr = ((n) - (f)->start_offset) + (f)->start))

// True if target is within current buffer
#define nk_contained(f, n) ((n) >= (f)->start_offset && (n) < (f)->start_offset + (f)->len)

// Set current file position and return character there
// n must be 0..file_size.  It's allowed to set file position to the end of the file, in which case -1 is returned
// Automatically uses fast version of fseek if requested position is within current buffer
#define nk_fseek(f, n) (nk_contained((f), (n)) ? nk_fseek_fast((f), (n)) : nk_fseek_slow((f), (n)))

// Relative set current file position and return character there
// n + nk_ftell(f) must be 0..file_size inclusive.
// It's allowed to set file position to the end of the file, in which case -1 is returned
// Automatically uses fast version of fseek if requested position is within current buffer
#define nk_fseek_rel(f, n) nk_fseek((f), nk_ftell(f) + (n))


// Read current character.  Only call if you know you're not already at end of file
//  Always returns valid file character
#define nk_fpeek_fast(f) (*(f)->ptr)

// Same as above, but you may call if you are already at end of file
//  Returns -1 if at end of file
#define nk_fpeek(f) (nk_feof(f) ? -1 : nk_fpeek_fast(f))

// Peek from a specified relative offset without changing file position
#define nk_fpeek_rel(f, n) (nk_contained((f), nk_ftell(f) + (n)) ? *((f)->ptr + (n)) : (((f)->tmp_pos = nk_ftell(f)), (f)->tmp = nk_fseek((f), nk_ftell(f) + (n)), nk_fseek((f), (f)->tmp_pos), (f)->tmp))

// Peek from a specified absolute offset without changing file position
#define nk_fpeek_abs(f, n) (nk_contained((f), (n)) ? *((f)->ptr + (n) - (f)->start_offset) : (((f)->tmp_pos = nk_ftell(f)), (f)->tmp = nk_fseek((f), (n)), nk_fseek((f), (f)->tmp_pos), (f)->tmp))


// Advance, then read character at that position
// Only safe to call if you already know that you're not at EOF to begin with
// Returns -1 if we end up at EOF
#define nk_fnext_fast(f) (++(f)->ptr == (f)->end ? nk_fseek_slow((f), (f)->start_offset + (f)->len) : nk_fpeek_fast(f))

// Advance, then read character at that position
// Returns -1 if we were already at EOF or if we end up at EOF
#define nk_fnext(f) (nk_feof(f) ? -1 : nk_fnext_fast(f))

// Read current character, then advance to next position
// Only safe to call if you know we're not already at end of file
// Always returns valid character
#define nk_fgetc_fast(f) (((f)->tmp = *(f)->ptr++), (nk_feof(f) ? (nk_fseek_slow((f), (f)->len + (f)->start_offset), (f)->tmp) : (f)->tmp))

// Read current character, then advance to next position
// Safe to call even if you start at end of file (returns -1 for this case)
#define nk_fgetc(f) (nk_feof(f) ? -1 : nk_fgetc_fast(f))


// Open a block device
nkinfile_t *nkinfile_open(
    nkinfile_t *f,
    size_t (*block_read)(
        void *block_read_ptr,
        size_t pos,
        unsigned char *buffer,
        size_t block_size
    ),
    void *block_read_ptr,
    size_t block_size,
    unsigned char *buffer
);

// Open a memory block as a file
nkinfile_t *nkinfile_open_mem(nkinfile_t *f, unsigned char *mem, size_t size);

// Open a string as a file
nkinfile_t *nkinfile_open_string(nkinfile_t *f, char *s);

// Print rest of file to nkstdout, usually for debugging

int nk_infile_print(nkinfile_t *f);

// Copy rest of infile to outfile

int nk_fcopy(nkoutfile_t *g, nkinfile_t *f);

extern nkinfile_t *nkinfile_null;

#endif
