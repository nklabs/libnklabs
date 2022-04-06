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

// Flash memory interface

#ifndef _Inkspiflash
#define _Inkspiflash

#include <stdint.h>

// Standard EEPROM/Flash commands

#define NK_FLASH_CMD_WRITE_ENABLE 0x06
#define NK_FLASH_CMD_WRITE_DISABLE 0x04

#define NK_FLASH_CMD_READ_STATUS 0x05
#define NK_FLASH_CMD_WRITE_STATUS 0x01
#define NK_FLASH_CMD_WRITE 0x02
#define NK_FLASH_CMD_READ 0x03


//

#define NK_FLASH_CMD_VOLATILE_WRITE_ENABLE 0x50

#define NK_FLASH_CMD_FAST_READ 0x0B

#define NK_FLASH_CMD_READ_STATUS_2 0x35
#define NK_FLASH_CMD_WRITE_STATUS_2 0x31
#define NK_FLASH_CMD_READ_STATUS_3 0x15
#define NK_FLASH_CMD_WRITE_STATUS_3 0x11

#define NK_FLASH_CMD_READ_JEDEC 0x9F
#define NK_FLASH_CMD_READ_UNIQUE 0x4B

#define NK_FLASH_CMD_ERASE_64K 0xD8
#define NK_FLASH_CMD_ERASE_32K 0x52
#define NK_FLASH_CMD_ERASE_4K 0x20

// Describe a particular SPI-flash

struct nk_spiflash_info
{
	// SPI interface
	int (*spi_transfer)(void *spi_ptr, uint8_t *data, uint32_t len);
	void *spi_ptr;

	// Pointer to transfer buffer
	// This must be page_size + addr_size + 1 or more
	uint8_t *buffer;

	// Information about SPI-flash or EEPROM
	uint32_t page_size; // Should be power of 2 but page_size + addr_size + 1 must not be larger than maximum spi_transfer size
	int busy_timeout;
	int addr_size; // 1, 2, 3 or 4

	int n_erase_options; // Number of available erase options.  0 means erase not required (EEPROM).

	// Table of erase options sorted with largest size first.

	struct {
		uint32_t erase_size;
		unsigned char erase_cmd;
	} erase_options[4];
};

// Write enable

int nk_flash_write_enable(const struct nk_spiflash_info *info);

// Write status

int nk_flash_write_status(const struct nk_spiflash_info *info, uint8_t val);

// Write disable

int nk_flash_write_disable(const struct nk_spiflash_info *info);

// Wait for not busy

int nk_flash_busy_wait(const struct nk_spiflash_info *info);

// Erase a region
// Region must start and end on a mutliple of the device's smallest erasable unit size
// Return 0 for success, -1 for error.

int nk_spiflash_erase(const struct nk_spiflash_info *info, uint32_t address, uint32_t byte_count);

// Write to flash. This handles any number for byte_count- it will break up the write
// into multiple page writes as necessary.
// Return 0 for success, -1 for error.
int nk_spiflash_write(const struct nk_spiflash_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);

// Read from flash.  address and byte_count can be any values- the flash memory
// automatically crosses page boundaries.
// Return 0 for success, -1 for error.
int nk_spiflash_read(const struct nk_spiflash_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);

// Hex dump of spi-flash
void nk_spiflash_hex_dump(const struct nk_spiflash_info *info, uint32_t addr, uint32_t len);

// Compute CRC on a region of flash
uint32_t nk_spiflash_crc(const struct nk_spiflash_info *info, uint32_t addr, uint32_t len);

// Generic user interface to SPI-flash or EEPROM
int nk_spiflash_command(const struct nk_spiflash_info *info, nkinfile_t *args);

#endif
