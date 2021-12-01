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

#include "nkoutfile.h"

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
) {
    f->start = f->ptr = buffer;
    f->size = len;
    f->end = f->start + f->size;
    f->block_write_ptr = block_write_ptr;
    f->block_write = block_write;
    return f;
}

int nk_fflush(nkoutfile_t *f)
{
    size_t len = f->ptr - f->start;
    f->ptr = f->start;
    return f->block_write(f->block_write_ptr, f->start, len);
}

static int block_write_overflow(void *ptr, unsigned char *buffer, size_t len)
{
    return -1;
}

nkoutfile_t *nkoutfile_open_mem(nkoutfile_t *f, char *mem, size_t size)
{
    return nkoutfile_open(f, block_write_overflow, NULL, (unsigned char *)mem, size);
}
