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
    const uint32_t buf_size; // Size of above buffer: power of 2 less than or equal to erase_size, but >= sizeof(nk_checked_header_t)
    const void *info; // First arg to flash access functions
    // Flash access functions
    // These should all return 0 for success
    int (* const flash_read)(const void *info, uint32_t addr, uint8_t *buf, uint32_t size);
    int (* const flash_erase)(const void *info, uint32_t addr, uint32_t size); // NULL for no erase
    int (* const flash_write)(const void *info, uint32_t addr, uint8_t *buf, uint32_t size);
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
int nk_checked_write(void *ptr, unsigned char *buffer, size_t len);

// Close write file: write header
int nk_checked_write_close(nk_checked_t *var_file);

// For nkinfile_t: read a block from the file
size_t nk_checked_read(void *ptr, unsigned char *buffer, size_t offset);

#endif
