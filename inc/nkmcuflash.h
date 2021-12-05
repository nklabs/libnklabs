#ifndef _Inkmcuflash
#define _Inkmcuflash

// Flash memory interface

// Return 0 for success, -1 for error.
int nk_init_mcuflash();

// Erase 1 or more sectors.  Address and byte_count should both be multiples of
// the sector size.
// Return 0 for success, -1 for error.
int nk_mcuflash_erase(uint32_t address, uint32_t byte_count);

// Write to flash. This handles any number for byte_count- it will break up the write
// into multiple page writes as necessary.
// Return 0 for success, -1 for error.
int nk_mcuflash_write(uint32_t address, uint8_t *data, uint32_t byte_count);

// Read from flash.  address and byte_count can be any values- the flash memory
// automatically crosses page boundaries.
// Return 0 for success, -1 for error.
int nk_mcuflash_read(uint32_t address, uint8_t *data, uint32_t byte_count);

#endif
