# Input file abstraction

## Files

[nkinfile.h](../inc/nkinfile.h),
[nkinfile.c](../src/nkinfile.c)

## Description

```c
nkinfile_t *nkinfile_open(
    nkinfile_t *f,
    size_t (*block_read)(
        void *block_read_ptr,
        size_t offset,
        unsigned char *buffer,
	size_t size
    ),
    void *block_read_ptr,
    size_t block_size,
    unsigned char *buffer
);

nkinfile_t *nkinfile_open_mem(nkinfile_t *f, unsigned char *mem, size_t size);

nkinfile_t *nkinfile_open_string(nkinfile_t *f, char *s);

size_t nk_ftell(nkinfile_t *f);

int nk_feof(nkinfile_t *f);

int nk_fseek(nkinfile_t *f, size_t pos);

int nk_fseek_rel(nkinfile_t *f, size_t rel_pos);

int nk_fpeek(nkinfile_t *f);

int nk_fpeek_rel(nkinfile_t *f, size_t rel_pos);

int nk_fpeek_abs(nkinfile_t *f, size_t pos);

int nk_fnext(nkinfile_t *f);
int nk_fnext_fast(nkinfile_t *f);

int nk_fgetc(nkinfile_t *f);
int nk_fgetc_fast(nkinfile_t *f);

int nk_infile_print(nkinfile_t *f);

int nk_fcopy(nkoutfile_t *g, nkinfile_t *f);
```

This is a thin wrapper that provides fast, low overhead file-like random
read access primitives to an underlying memory area, C-string or block
device.  It is used to direct input to __nk_fscanf__.  __nk_fscanf__
requires random access since it restores the byte offset to the original
location (before __nk_fscan__ was called) when it fails to parse.

An nkinfile_t may be opened in the following ways:

__nkinfile_open_string__ opens a NUL-terminated C string as a file.  It
calls __strlen__ to find the length of the string.

__nkinfile_open_mem__ opens a memory area of a given length as a file.

__nkinfile_open__ opens some kind of block device as a file.  __block_read__
is a function you provide which should read a block from the device at a
given __offset__ (which will be a multiple of __block_size__) to the
specified memory buffer.  __block_read_ptr__ is a void pointer passed
unchanged as the first argument of __block_read__ whenever it is called. 
__block_size__ specifies the device's block size, which is the number of
bytes that should be transferred by a single call to __block_read__. 
__buffer__ is a memory buffer that you provide that must be large enough
for __block_size__ bytes.

Note that __block_read__ is immediately called by __nkinfile_open__ to load
the first block of the file into the buffer.

__block_read__ must return the number of bytes read.  This should always be
equal to __block_size__ except for when the last block of the file (or one
past the last block of the file for the case where the last block contains
__block_size__ bytes) is read.  This is how the end of file is indicated to
the nkinfile wrapper.

Note that __block_read__ may be called with any valid position (any position
which is a multiple of the block size) in any order.  Note that even if the
last block has been read, blocks from other offsets may still be
subsequently requested.

__nkinfile_open__, __nkinfile_open_mem__ and __nkinfile_open_string__ all
return the __nkinfile_t__ pointer.

Note that there is no file close operation.  The __nkinfile_t__ does not
allocate any resources that need to be freed or released.  Releasing the
buffer (if necessary) is up to you.

__nk_feof__ returns true if the nkinfile_t byte offset is at the end of the
file.  __nk_feof__ is implemented as a macro.

__nk_ftell__ returns the byte offset of the nkinfile_t.  __nk_ftell__ is
implemented as a macro.

__nk_fseek__ repositions the nkinfile_t to the requested byte offset.  The
byte offset must be in the range 0 to the size of the file inclusive.  If
the file size is 4 bytes, then the possible valid byte offsets are 0, 1, 2,
3 and 4.  __nk_fseek__ is implemented as a macro.

__nk_fseek_rel__ is like __nk_fseek__, except that it performs a relative
seek- that is, it repositions the nkinfile_t to nk_ftell(f) + rel_pos. 
nk_ftell(f) + rel_pos must be in the range 0 to the size of the file
inclusive.

__nk_fpeek__ reads a single unsigned byte from the input file, without
changing the current byte offset.  If the byte offset is at the end of the
file, __nk_fpeek__ returns -1.  __nk_fpeek__ is implemented as a macro.

__nk_fseek_rel__ is like __nk_fpeek__ except that it reads from nk_ftell(f) + __rel_pos__.

__nk_fseek_abs__ is like __nk_fpeek__ except that it reads the byte at byte
offset __pos__.

__nk_fnext__ advances the byte offset by 1, and returns the byte at that
location.  It returns -1 if the new byte offset is at the end of the file. 
Note that if the byte offset was already at the end, __nk_fnext__ does not
change the offset and it returns -1.  __nk_fnext_fast__ may be used if it is
known that the byte offset is not already at the end of the file (via a
previous call to __nk_feof__ or __nk_fpeek__).  __nk_fnext__ and
__nk_fnext_fast__ are implemented as macros.

__nk_fgetc__ reads the byte at the current byte offset, and then advances
the offset by 1.  Note that if the offset was at the end of the file,
__nk_fgetc__ returns -1 and does not change the offset.  __nk_fgetc_fast__
may be used if it is known that the byte offset is not already at the end
(via a previous call to __nk_feof__ or __nk_fpeek__).  __nk_fgetc__ and
__nk_fgetc_fast__ are implemented as macros.

__nk_fcopy__ copies the remainder of the nkinfile_t to the specified
nkoutfile_t.

__nk_infile_print__ is the same as __nk_fcopy__ except that it copies from
the specified __nkinfile_t__ to __nkstdout__.  It prints the remainder of the
nkinfile_t.
