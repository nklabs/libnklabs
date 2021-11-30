// File output abstraction

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

#ifndef _nkoutfile
#define _nkoutfile

#include <stdlib.h> // For size_t

// Nkfile pointer

typedef struct nkoutfile nkoutfile_t;

struct nkoutfile
{
    // Information about output buffer
    unsigned char *ptr; // Current pointer into buffer
    unsigned char *start; // Start of buffer
    unsigned char *end; // End of buffer
    size_t size; // Size of buffer

    // For abstracting some kind of fixed-length block-based storage system into a file
    void *block_write_ptr; // Pointer to pass to block_read
    int (*block_write)(void *block_write_ptr, unsigned char *buffer, size_t len);
};

// Write character to file, flush if buffer is full
// Returns return value of block_write or 0 if no flushing was needed.
// Argument c is accessed only once, safe to used code with side-effects.
// Argument f is accessed multiple times, not safe to use code with side-effects.
#define nk_fputc(f, c) (*(f)->ptr++ = (c), (f)->ptr == (f)->end ? (f)->block_write((f)->block_write_ptr, (f)->ptr = (f)->start, (f)->size) : 0)

// Flush buffered output.  Returns return value of block_write().
// Note that block_write() is called with a length of 0 if there is no data to flush.
int nk_fflush(nkoutfile_t *f);

// Open a block device
nkoutfile_t *nkoutfile_open(
    nkoutfile_t *f,
    int (*block_write)(
        void *block_write_ptr,
        unsigned char *buffer,
        size_t len
    ),
    void *block_write_ptr,
    unsigned char *buffer,
    size_t len
);

// Open a memory block as a file
nkoutfile_t *nkoutfile_open_mem(nkoutfile_t *f, char *mem, size_t size);

#endif
