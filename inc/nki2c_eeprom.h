// Flash memory interface

#ifndef _Inki2c_eeprom
#define _Inki2c_eeprom

#include <stdint.h>

// Describe a particular I2C EEPROM

struct nk_i2c_eeprom_info
{
	// How to access this device's I2C bus
	int (*i2c_write)(void *i2c_ptr, uint8_t addr, size_t len, const uint8_t *buf);
	int (*i2c_read)(void *i2c_ptr, uint8_t addr, size_t len, uint8_t *buf);
	void *i2c_ptr;

	// This device's I2C address
	uint8_t i2c_addr;

	// Pointer to transfer buffer
	// This must be page_size + addr_size or more
	uint8_t *buffer;

	// Information about SPI-flash or EEPROM
	// page_size should be power of 2
	// page_size + addr_size + 1 must not be larger than maximum i2c transfer size
	// It's OK if page_size is less than the device's actual page size
	uint32_t page_size; 

	// Number of times to poll chip before timeout
	int busy_timeout;

	// Number of address bytes
	int addr_size; // 1 or 2
};

// Wait for not busy

int nk_i2c_eeprom_busy_wait(const struct nk_i2c_eeprom_info *info);

// Write to EEPROM. This handles any number for byte_count- it will break up the write
// into multiple page writes as necessary.
// Return 0 for success, -1 for error.
int nk_i2c_eeprom_write(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);

// Read from flash.  address and byte_count can be any values- the flash memory
// automatically crosses page boundaries.
// Return 0 for success, -1 for error.
int nk_i2c_eeprom_read(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);

// Hex dump of EEPROM
void nk_i2c_eeprom_hex_dump(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len);

// Compute CRC on a region of EEPROM
uint32_t nk_i2c_eeprom_crc(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len);

// Generic user interface to EEPROM
int nk_i2c_eeprom_command(const struct nk_i2c_eeprom_info *info, nkinfile_t *args, uint32_t *old_addr);

#endif
