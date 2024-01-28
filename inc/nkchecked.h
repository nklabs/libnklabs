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

// A checked file in region of flash memory

// These functions allow you to reserve a contiguous region of flash memory
// for a single file accessible through nkinfile and nkoutfile.  They add a
// small header to the file data that contains the file length and CRC.

#ifndef _Inkchecked
#define _Inkchecked

#include <stdlib.h>
#include <stdint.h>

// File header

typedef struct {
    uint32_t size; // Size of file
    uint32_t crc; // CRC of file
} nk_checked_header_t;

// File access structure: constant part

typedef struct {
    const uint32_t area_size; // Size of flash area
    const uint32_t area_base; // Location of flash area
    const uint32_t erase_size; // Flash erase size
    const void *info; // First arg to flash access functions
    // Flash access functions
    // These should all return 0 for success
    int (* const flash_read)(const void *info, uint32_t addr, uint8_t *buf, size_t size);
    int (* const flash_erase)(const void *info, uint32_t addr, uint32_t size); // NULL for no erase
    int (* const flash_write)(const void *info, uint32_t addr, const uint8_t *buf, size_t size);
    const size_t granularity;
} nk_checked_base_t;

// File access structure: variable part

typedef struct {
    const nk_checked_base_t *file;
    uint32_t crc; // Current file CRC
    uint32_t size; // Size of file (for reading)
} nk_checked_t;

// Open file for reading
// This function checks the file's integrity
// Returns false if file is corrupted: incorrect length or CRC
int nk_checked_read_open(nk_checked_t *var_file, const nk_checked_base_t *file, unsigned char *buffer, size_t buf_size);

// Open file for writing
int nk_checked_write_open(nk_checked_t *var_file, const nk_checked_base_t *file);

// For nkoutfile_t: write a block to the file
int nk_checked_write(nk_checked_t *ptr, size_t offset, const unsigned char *buffer, size_t len);

// Close write file: write header
int nk_checked_write_close(nk_checked_t *var_file);

// For nkinfile_t: read a block from the file
size_t nk_checked_read(nk_checked_t *ptr, uint32_t offset, unsigned char *buffer, size_t block_size);

#endif
