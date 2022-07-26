# Maintain a file in a contiguous region of flash memory

This module allows a region of flash memory to be used as a file.  The
region has only the file's data, no header.  It is used, for example, by
nkymodem to locate a transferred FPGA image file in flash memory.

See also: [nkchecked](nkdirect.md) for a file in flash memory protected by CRC.

## Files

[nkdirect.h](../inc/nkdirect.h),
[nkdirect.c](../src/nkdirect.c)

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
} nk_direct_base_t;

int nk_direct_read_open(nk_direct_t *var_file, const nk_direct_base_t *file, unsigned char *buffer, size_t buf_size);

size_t nk_direct_read(void *ptr, size_t offset, unsigned char *buffer, size_t block_size);

int nk_direct_write_open(nk_direct_t *var_file, const nk_direct_base_t *file);

int nk_direct_write(void *ptr, unsigned char *buffer, size_t len);

int nk_direct_write_close(nk_direct_t *var_file);

```

The file is stored without any header in a contiguous range of memory
accessible via the given flash_read, flash_erase and flash_write functions. 

When the file is opened for reading, the file size is 'area_size'.

When the file is opened for writing, the maximum file size is 'area_size'-
writes beyond this point are truncated.  The nk_direct_t structure holds a
running CRC of the data written.

Once a file has been opened, nk_checked_read can be used to read a range of
data from the file into a memory buffer.  This function is intended to be
useable as the block read function of nkinfile_t.

nk_checked_read returns the number of bytes read.  If the asked for range
goes past the end of the file, the returned value will be less than the
block_size parameter.  0 is returned if any read error occurred.

Note that nk_checked_write is not buffered: it calls the given flash_write
function to write the given data to the memory.  If the memory has a minimum
write size (as is the case for the on-die MCU flash of STM32), it is up to
you to ensure that calls to nk_checked_write are multiples of this size.
