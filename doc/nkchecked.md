# Maintain a CRC-checked file in a contiguous region of flash memory

This module implements a simple single file store in flash memory.  This is
useful for cases where a full filesystem is not needed.  It is used by
nkdbase to locate the serialized database in flash memory.

See also [nkdirect](nkdirect.md) to treat a region of flash memory as a
file, but without any header.

## Files

[nkchecked.h](../inc/nkchecked.h),
[nkchecked.c](../src/nkchecked.c)

## Description

```c
typedef struct {
    const uint32_t area_size; // Size of flash area
    const uint32_t area_base; // Location of flash area
    const uint32_t erase_size; // Flash erase size
    const void *info; // First arg to flash access functions
    // Flash access functions
    // These should all return 0 for success
    int (* const flash_read)(const void *info, uint32_t addr, uint8_t *buf, uint32_t size);
    int (* const flash_erase)(const void *info, uint32_t addr, uint32_t size); // NULL for no erase
    int (* const flash_write)(const void *info, uint32_t addr, uint8_t *buf, uint32_t size);
} nk_checked_base_t;

int nk_checked_read_open(nk_checked_t *var_file, const nk_checked_base_t *file, unsigned char *buffer, size_t buf_size);

size_t nk_checked_read(void *ptr, size_t offset, unsigned char *buffer, size_t block_size);

int nk_checked_write_open(nk_checked_t *var_file, const nk_checked_base_t *file);

int nk_checked_write(void *ptr, unsigned char *buffer, size_t len);

int nk_checked_write_close(nk_checked_t *var_file);

```

The file is stored along with a small header in a contiguous range of memory
accessible via the given flash_read, flash_erase and flash_write functions. 
The header holds the exact file size and a CRC of contents of the file.  The
header is written only when the file is closed.  The CRC is checked when the
file is opened for reading, to give assurance that the file is valid.  In
particular, it will detect the case of a power failure occurring between
open is called and close has completed.

When nk_checked_read_open is called, the CRC of the file is verified.  The
CRC of the file is computed and compared with the CRC saved in the file's
header.  If all looks good, 0 is returned.  If CRC doesn't match, or if the
length of the file makes no sense, or if flash_read returned and error, then
nk_checked_read_open returns a non-zero value.

Once a file has been opened, nk_checked_read can be used to read a range of
data from the file into a memory buffer.  This function is intended to be
useable as the block read function of nkinfile_t.

nk_checked_read returns the number of bytes read.  If the asked for range
goes past the end of the file, the returned value will be less than the
block_size parameter.  0 is returned if any read error occurred.

nk_checked_write_open prepares the flash memory for writing.  It erases the
first block of the flash memory and initializes the nk_checked_t structure. 
0 is returned for success, or non-zero otherwise.

nk_checked_write appends a block of data to the file.  The CRC and file size
variables in nk_checked_t are updated.  nk_checked_write returns 0 if there
are no errors.  It returns non-zero on error. 

Note that nk_checked_write is not buffered: it calls the given flash_write
function to write the given data to the memory.  If the memory has a minimum
write size (as is the case for the on-die MCU flash of STM32), it is up to
you to ensure that calls to nk_checked_write are multiples of this size.

nk_checked_write_close writes the header to the flash memory.  The header is
written to the very beginning of the flash memory area reserved for the
file.  This was the area erased when nk_checked_write_open was called. 
nk_checked_write_close returns 0 for success.
