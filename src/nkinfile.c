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
        f->len = f->block_read(f->block_read_ptr, f->start_offset, f->start, f->block_size);
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
    f->start = buffer;
    f->ptr = f->start;
    f->start_offset = 0;
    
    f->block_read_ptr = block_read_ptr;
    f->block_read = block_read;
    f->block_size = block_size;
    f->len = block_size;

    // Read first block into buffer
    if (block_read)
        f->len = block_read(block_read_ptr, f->start_offset, buffer, f->block_size);

    f->end = f->start + f->len;

    return f;
}

nkinfile_t *nkinfile_open_mem(nkinfile_t *f, unsigned char *mem, size_t size)
{
    nkinfile_open(f, NULL, NULL, size, mem);
    f->block_size = 0;
    return f;
}

nkinfile_t *nkinfile_open_string(nkinfile_t *f, char *s)
{
    return nkinfile_open_mem(f, (unsigned char *)s, strlen(s));
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
