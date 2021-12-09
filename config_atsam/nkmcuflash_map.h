// Locate example database in MCU flash memory

#ifdef STM32F030x8

// We only have 64K of flash, so stick the database in the last two pages

#define NKDBASE_BANK0 0xF800
#define NKDBASE_BANK1 0xFC00
#define NKDBASE_SIZE 1024

#else

// Put database at 96K

#define NKDBASE_BANK0 0x1c000
#define NKDBASE_BANK1 0x1e000
#define NKDBASE_SIZE 8192

#endif
