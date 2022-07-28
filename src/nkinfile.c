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

#include <string.h>
#include "nkprintf.h"
#include "nkinfile.h"

int nk_fseek_slow(nkinfile_t *f, size_t pos)
{
    size_t offset;

    // Requested position fits in current buffer
    if (pos >= f->start_offset && pos < f->start_offset + f->len)
    {
        f->ptr = f->start + (pos - f->start_offset);
        return *f->ptr;
    }

    // Nope, load necessary block
    offset = (pos & (f->block_size - 1));
    f->start_offset = pos - offset;

    if (f->block_read)
    {
        // Load block.  If end of file is within block, f->len will be less than block_size.
        // If end of file is exactly at start of block, then nothing is loaded and f->len is zero.
        // Otherwise f->len will be equal to block_size.
        f->len = f->block_read(f->block_read_ptr, f->start_offset, f->buffer, f->block_size);
    }
    else
    {
        // We don't do this, because when we have opened a memory block, we want seek to work
        // even after we've reached EOF. 
        // f->len = 0;
    }
    f->end = f->start + f->len;

    if (offset < f->len)
    {
        // Offset is valid
        f->ptr = f->start + offset;
        return *f->ptr;
    }
    else
    {
        // Tried to seek past the end?
        f->ptr = f->start + f->len;
        return -1;
    }
}

nkinfile_t *nkinfile_open(nkinfile_t *f, size_t (*block_read)(void *block_read_ptr, size_t pos, unsigned char *buffer, size_t block_size), void *block_read_ptr, size_t block_size, unsigned char *buffer)
{
    f->buffer = buffer;
    f->start = buffer;
    f->ptr = f->start;
    f->start_offset = 0;
    
    f->block_read_ptr = block_read_ptr;
    f->block_read = block_read;
    f->block_size = block_size;
    f->len = block_size;

    // Read first block into buffer
    if (block_read)
        f->len = block_read(block_read_ptr, f->start_offset, f->buffer, f->block_size);

    f->end = f->start + f->len;

    return f;
}

nkinfile_t *nkinfile_open_mem(nkinfile_t *f, const unsigned char *mem, size_t size)
{
    f->buffer = 0;
    f->start = mem;
    f->ptr = f->start;
    f->start_offset = 0;
    
    f->block_read_ptr = NULL;
    f->block_read = NULL;
    f->block_size = 0;
    f->len = size;

    f->end = f->start + f->len;

    return f;
}

nkinfile_t *nkinfile_open_string(nkinfile_t *f, const char *s)
{
    return nkinfile_open_mem(f, (const unsigned char *)s, strlen(s));
}

int nk_fcopy(nkoutfile_t *g, nkinfile_t *f)
{
    int status = 0;
    while (!nk_feof(f))
        status |= nk_fputc(g, nk_fgetc(f));
    return status;
}

int nk_infile_print(nkinfile_t *f)
{
    return nk_fcopy(nkstdout, f);
}

static nkinfile_t _nkinfile_null =
{
    .buffer = 0,
    .ptr = 0,
    .start = 0,
    .end = 0,
    .len = 0,
    .start_offset = 0,
    .block_read_ptr = 0,
    .block_read = 0,
    .block_size = 0
};

nkinfile_t *nkinfile_null = &_nkinfile_null;
