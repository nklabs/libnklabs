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

// A file in region of flash memory

// These functions allow you to reserve a contiguous region of flash memory
// for a single file accessible through nkinfile and nkoutfile.  There is no
// header for the file in flash.

#ifndef _Inkdirect
#define _Inkdirect

#include <stdlib.h>
#include <stdint.h>

// File access structure: constant part

typedef struct {
    const uint32_t area_size; // Size of flash area
    const uint32_t area_base; // Location of flash area
    const uint32_t erase_size; // Flash erase size
    const void *info; // First arg to flash access functions
    // Flash access functions
    // These should all return 0 for success
    int (* const flash_read)(const void *info, uint32_t addr, uint8_t *buf, uint32_t size);
    int (* const flash_erase)(const void *info, uint32_t addr, uint32_t size); // NULL for no erase
    int (* const flash_write)(const void *info, uint32_t addr, const uint8_t *buf, uint32_t size);
    const size_t granularity;
} nk_direct_base_t;

// File access structure: variable part

typedef struct {
    const nk_direct_base_t *file;
    uint32_t crc; // Current file CRC
    uint32_t size; // Size of file (for reading)
} nk_direct_t;

// Open file for reading
int nk_direct_read_open(nk_direct_t *var_file, const nk_direct_base_t *file, unsigned char *buffer, size_t buf_size);

// Open file for writing
int nk_direct_write_open(nk_direct_t *var_file, const nk_direct_base_t *file);

// For nkoutfile_t: write a block to the file
int nk_direct_write(nk_direct_t *ptr, const unsigned char *buffer, size_t len);

// Close write file
int nk_direct_write_close(nk_direct_t *var_file);

// For nkinfile_t: read a block from the file
size_t nk_direct_read(nk_direct_t *ptr, size_t offset, unsigned char *buffer, size_t block_size);

#endif
