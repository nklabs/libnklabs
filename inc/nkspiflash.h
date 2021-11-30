// Flash memory interface

#ifndef _Inkspiflash
#define _Inkspiflash

// Return 0 for success, -1 for error.
int nk_init_spiflash();

// Get sector size.  This is the minimum size that nk_flash_erase can erase.
//uint32_t nk_flash_sector_size();
#define NK_SPIFLASH_ERASE_SECTOR_SIZE 65536

// Erase 1 or more sectors.  Address and byte_count should both be multiples of
// the sector size.
// Return 0 for success, -1 for error.
int nk_spiflash_erase(uint32_t address, uint32_t byte_count);

// Write to flash. This handles any number for byte_count- it will break up the write
// into multiple page writes as necessary.
// Return 0 for success, -1 for error.
int nk_spiflash_write(uint32_t address, uint8_t *data, uint32_t byte_count);

// Read from flash.  address and byte_count can be any values- the flash memory
// automatically crosses page boundaries.
// Return 0 for success, -1 for error.
int nk_spiflash_read(uint32_t address, uint8_t *data, uint32_t byte_count);

// Flash locations

// FPGA attemps the load FLASH_PFGA_BUFFER_MAIN first.  If it has bad CRC, it then loads
// FLASH_GOLDEN_JUMP which commands it to load from FLASH_FPGA_BUFFER_GOLDEN.

// There are two copies of the RISC-V firmware: FLASH_FIRMWARE_BUFFER_0 and FLASH_FIRMWARE_BUFFER_1.
// Bit 0 of first byte of FLASH_FIRMWARE_FLAG indicates which of these will be used for
// execute in place on next boot up.

#define FLASH_FIRMWARE_BUFFER (0)
#define FLASH_FIRMWARE_BUFFER_SIZE (0x20000)

// Calibration store
#define FLASH_KEYVAL_SIZE 8192
#define FLASH_CAL_KEYVAL_ADDR_0 (0x100000 + 0)
#define FLASH_CAL_KEYVAL_ADDR_1 (0x100000 + 0x10000)

// Consumption log
#define FLASH_LOG_ADDR (0x100000 + 0x20000)
#define FLASH_LOG_SIZE 0x40000

// Configuration store
#define FLASH_KEYVAL_SIZE 8192
#define FLASH_CONFIG_KEYVAL_ADDR_0 (0x100000 + 0x60000 + 0)
#define FLASH_CONFIG_KEYVAL_ADDR_1 (0x100000 + 0x60000 + 0x10000)

// We want the SPI-flash to last a long time, but we write to it every LOGPERIOD ms (currently once a minute)
// Log entry is 64 bytes
// 1024 writes to fill a sector
// Four sectors, gives 4096 writes
// We write to all sectors every 4096 * LOGPERIOD / 60000 minutes (currently 4096 minutes).
// SPI-flash lasts 100000 writes, so it will last 100000 * 4096 minutes.

// Only lower 16-bits of these make it to S2868A1_EEPROM_ReadPage
#define FLASH_FAKE_BOARD_DATA 0x801F000
#define FLASH_FAKE_SIGFOX_DATA 0x801E000

#define FLASH_BOARD_DATA 0xF000
#define FLASH_SIGFOX_DATA 0xE000

#endif
