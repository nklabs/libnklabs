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

#include <string.h> // For memcpy
#include "nkoutfile.h"

nkoutfile_t *nkoutfile_open(
    nkoutfile_t *f,
    int (*block_write)(
        void *block_write_ptr,
        size_t offset,
        const unsigned char *buffer,
        size_t len
    ),
    void *block_write_ptr,
    unsigned char *buffer,
    size_t len,
    size_t granularity
) {
    f->start = f->ptr = buffer;
    f->size = len;
    if (f->start)
        f->end = f->start + f->size;
    else
        f->end = f->start;
    f->block_write_ptr = block_write_ptr;
    f->block_write = block_write;
    f->granularity = granularity;
    f->start_offset = 0;
    return f;
}

int nk_fflush(nkoutfile_t *f)
{
    int rtn;
    size_t len = (size_t)(f->ptr - f->start);
    // Always reposition
    f->ptr = f->start;
    // Round write up to granularity
    len = ((len + (f->granularity - 1)) & ~(f->granularity - 1));
    if (f->block_write)
    {
        rtn = f->block_write(f->block_write_ptr, f->start_offset, f->start, len);
        f->start_offset += len;
    }
    else
    {
        // No flush function
        rtn = -1;
    }
    return rtn;
}

nkoutfile_t *nkoutfile_open_mem(nkoutfile_t *f, char *mem, size_t size)
{
    // We have a buffer, but no output function
    return nkoutfile_open(f, NULL, NULL, (unsigned char *)mem, size, 1);
}

// This is called when we try to write to a full output buffer

int _nk_flush_and_putc(nkoutfile_t *f, int c)
{
    int rtn = 0;
    if (f->start)
    {
        // Normal case, we have a buffer
        // nk_fflush always resets ptr
        rtn = nk_fflush(f);
        *f->ptr++ = (unsigned char)c;
    }
    else if (f->block_write)
    {
        // Special case: no buffering, just call output function directly
        // If granularity is not 1, we write whatever junk follows d on the stack
        unsigned char d = (unsigned char)c;
        rtn = f->block_write(f->block_write_ptr, f->start_offset, &d, f->granularity);
        f->start_offset += f->granularity;
    }
    else
    {
        // No buffer and no output function?
        // Just discard the data, not an error.
    }
    return rtn;
}

int nk_fwrite(nkoutfile_t *f, const unsigned char *buf, size_t len)
{
    int rtn = 0;
    if (f->start)
    {
        // Normal case, we have an output buffer
        while (len)
        {
            if (f->ptr == f->end)
            {
                rtn |= nk_fflush(f);
            }
            size_t space = f->end - f->ptr;
            size_t now;
            if (len < space)
                now = len;
            else
                now = space;
            memcpy(f->ptr, buf, now);
            f->ptr += now;
            buf += now;
            len -= now;
        }
    }
    else if (f->block_write)
    {
        // No output buffer case, call block_write now
        while (len)
        {
            size_t now;
            if (len < f->size)
                now = len;
            else
                now = f->size;
            size_t roundup = ((now + (f->granularity - 1)) & ~(f->granularity - 1));
            rtn |= f->block_write(f->block_write_ptr, f->start_offset, buf, roundup);
            f->start_offset += roundup;
            buf += now;
            len -= now;
        }
    }
    else
    {
        // No buffer and no output function?
        // Just discard the data, not an error.
    }
    return rtn;
}
