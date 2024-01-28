# Output file abstraction

## Files

[nkoutfile.h](../inc/nkoutfile.h),
[nkoutfile.c](../src/nkoutfile.c)

## Description

```c
extern nkoutfile_t *nkstdout;
extern nkoutfile_t *nkstderr;
extern nkoutfile_t *nkstdnull;

nkoutfile_t *nkoutfile_open(
    nkoutfile_t *f,
    int (*block_write)(
        void *block_write_ptr,
        size_t offset,
        unsigned char *buffer,
        size_t len
    ),
    void *block_write_ptr,
    unsigned char *buffer,
    size_t len,
    size_t granularity
);

nkoutfile_t *nkoutfile_open_mem(nkoutfile_t *f, unsigned char *mem, size_t size);

int nk_fputc(nkoutfile_t *f, unsigned char c);

int nk_fwrite(nkoutfile_t *f, const char *buf, size_t len);

int nk_fflush(nkoutfile_t *f);

size_t nk_fnote(nkoutfile_t *f);
```

This is a thin wrapper that provides fast, low overhead file-like optionally
buffered sequential write access primitives to an underlying memory area or
block device.  It is used to direct output from __nk_fprintf__ to a
particular device or memory region.

An nkoutfile_t may be opened in the following ways:

__nkoutfile_open_mem__ opens a fixed-length memory area for writing.  If you
try to write more bytes than there is space, then __nk_fputc__ returns -1 to
indicate this error.

__nkoutfile_open__ opens a block device for writing.  __block_write__ is a
function that is called whenever the buffer is full.  __block_write_ptr__ is
a void pointer that is passed unchanged as the first argument to
__block_write__.  __buffer__ is the address of a memory area with space for
__len__ bytes, or NULL if not output buffered is desired.  __len__ is the
block size.  __granularity__ is the minimum alloweed write size for
block_write.

__nkoutfile_open__ may be called with NULL for __buffer__.  In this
case, there is no output buffering, instead each call to __nk_fputc__ or
__nk_fwrite__ causes a call to __block_write__.  In this case, __len__ is
the maximum number of bytes passed to __block_write__ (__nk_fwrite__ will
call __block_write__ multiple times if necessary to write all of the data).

When __block_write__ is called, its __offset__ argument has the file offset
to write, its __buffer__ argument has the start of the data to write, and
its __len__ argument has the number of bytes to write.  __block_write__ may
return an error status.  A value of zero means no error.

For each call to __block_write__, its __offset__ argument is set to the
amount of data previously written (since the initial call to
__nkoutfile_open__).

__nkoutfile_open__ and __nkoutfile_open_mem__ do not allocate any resources
which need to be freed or released, so there is no provided close operation. 
It is up to you to free the buffer (if necessary).

__nkoutfile_open__ and __nkoutfile_open_mem__ return the nkoutfile_t
pointer.

__nk_fputc__ appends a byte to the buffer.  If the buffer becomes full, it
calls the __block_write__ function provided in __nkoutfile_open__ to write
out the buffer.  If __nk_fputc__ calls __block_write__ it returns
__block_write's__ return value, otherwise it returns 0.  
__nk_fputc__ is implemented as an inline function or macro.

__nk_fwrite__ appends a block of memory to the buffer.  It calls
__block_write__ as necessary when the buffer becomes full.  __nk_fwrite__
returns __block_write's__ return value, otherwise it returns 0.

__nk_fnote__ returns the number of bytes written with __nk_fputc__ and
__nk_fwrite__ since the initial call to __nkoutfile_open__.

__nk_fflush__ calls __block_write__ with any remaining bytes to write out. 
In this case, __block_write's__ __len__ argument can be anything from 0 to
one less than the block size.  The write size is rounded up to be a multiple
of __granularity__.  __block_write__ is called even if there are no bytes to
write, so it's possible for the __len__ argument to be zero.

__nkstdout__ is the __nkoutfile_t__ used by __nk_printf__ to print to a
standard console device.  It is normally set up to use __nk_putc__ to write
bytes to the console.

__nkstderr__ is the __nkoutfile_t__ used by __nk_fscan_keyval__ to print
errors detected during parsing.

__nkstdnull__ is an __nkoutfile_t__ that can be used to discard output.
