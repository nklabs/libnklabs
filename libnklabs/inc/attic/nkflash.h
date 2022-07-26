// Copyright 2020 NK Labs, LLC

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

#ifndef _NKFLASH_H
#define  _NKFLASH_H

// Flash memory interface

// Return 0 for success, -1 for error.
int nk_init_flash();

// Get sector size.  This is the minimum size that nk_flash_erase can erase.
uint32_t nk_flash_sector_size();

// Erase 1 or more sectors.  Address and byte_count should both be multiples of
// the sector size.
// Return 0 for success, -1 for error.
int nk_flash_erase(uint32_t address, uint32_t byte_count);

// Write to flash. This handles any number for byte_count- it will break up the write
// into multiple page writes as necessary.
// Return 0 for success, -1 for error.
int nk_flash_write(uint32_t address, uint8_t *data, uint32_t byte_count);

// Read from flash.  address and byte_count can be any values- the flash memory
// automatically crosses page boundaries.
// Return 0 for success, -1 for error.
int nk_flash_read(uint32_t address, uint8_t *data, uint32_t byte_count);

#define NK_ERASE_SECTOR_SIZE 65536

// Flash locations

#define FLASH_FIRMWARE_BUFFER 0
#define FLASH_FIRMWARE_BUFFER_SIZE (1024*1024*16)

#define FLASH_FPGA_BUFFER (1024*1024*16)
#define FLASH_FPGA_BUFFER_SIZE (1024*1024*16)

#define FLASH_FIRMWARE_FLAG (1024*1024*32)

#endif    /* _NKFLASH_H   */
