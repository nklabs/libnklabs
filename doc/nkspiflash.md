# SPI-Flash / SPI-EEPROM driver

This is a simple driver for most SPI flash and EEPROM memory devices.

## Files

[nkspiflash.h](../inc/nkspiflash.h), [nkspiflash.c](../src/nkspiflash.c)

## nk_spiflash_info structure

You initialize a structure containing information about the device as well
as a pointer to a function which performs a full-duplex SPI transfer to the
device.  The address of this structure should be given as the first argument
to the provided memory access functions.

~~~c
// Describe a particular SPI-flash

struct nk_spiflash_info
{
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
~~~

__spi_transfer__ is a pointer to a function which performs a full-duplex
SPI-transaction to the memory device.  It should read and write __len__ bytes
to the device.  The write data is given in the array __data__.  The read data
is returned in the same array.  __spi_ptr__ is passed unchanged as the first
argument to this function.  __spi_transfer__ should return 0 for success or
any other value for error.
 
__buffer__ is the address of a memory buffer that can be used as the transfer
buffer for SPI transactions.  It will be used as the __data__ argument for
__spi_transfer__.  __buffer__ must be equal to or larger than __page_size__ + __addr_size__ + 1.

__page_size__ should be a power of 2.  Memory reads and writes are broken up
into transactions no larger than __page_size__.  __page_size__ +
__addr_size__ + 1 must not be larger than the maximum transfer size allowed
by __spi_transfer__ and must fit in the provided __buffer__.

__busy_timeout__ is the number of times the read status command (command
0x05) is issued when waiting for erase and writes to complete before timing
out with an error.

__addr_size__ is the number of address bytes required by the memory device. 
It is in the range 1 to 4, depending on the memory size.

__n_erase_options__ gives the number of different erase commands provided by
the memory device.  This can be 0 (for EEPROMs) to 4.

__erase_options__ is an array giving each erase command available to the
memory device.  Each entry should have the command code and its
corresponding erase size.  The array should be sorted from largest size to
smallest.  When the __nk_spiflash_erase__ function is given, the erase
request is fulfilled using the least number of erase commands possible.


## nk_flash_write_enable

~~~c
int nk_flash_write_enable(const struct nk_spiflash_info *info);
~~~

Issue the write enable command to the memory device (command code 0x06). 
You do not normally have to call this function.

Returns 0 for success.

## nk_flash_write_status

~~~c
int nk_flash_write_status(const struct nk_spiflash_info *info, uint8_t val);
~~~

Issue the write status command (command code 0x01) to the memory device with
the given status value.  You do not normally have to call this function.

Returns 0 for success.

## nk_flash_write_disable

~~~c
int nk_flash_write_disable(const struct nk_spiflash_info *info);
~~~

Issue the write disable command (command code 0x04) to the memory device.

Returns 0 for success.

## nk_flash_busy_wait

~~~c
int nk_flash_busy_wait(const struct nk_spiflash_info *info);
~~~

Repeatedly issue the read status command (command code 0x05) until the
memory device indicates that it is no longer busy.  You do normally have to
call this function.

Returns 0 for success, or -1 for timeout.

## nk_spiflash_erase

~~~c
int nk_spiflash_erase(const void *info, /* const struct nk_spiflash_info *info, */ uint32_t address, uint32_t byte_count);
~~~

Erase a region of flash memory.  Region must start and end on a mutliple of the device's smallest erasable unit
size.  nk_spiflash_erase issues the write enable command, so you don't have
to do it.  nk_spiflash_erase waits until the erase is complete before
returning.

Returns 0 for success, -1 for error.

## nk_spiflash_write

~~~c
int nk_spiflash_write(const void *info, /* const struct nk_spiflash_info *info, */ uint32_t address, uint8_t *data, uint32_t byte_count);
~~~

Write __byte_count__ bytes to the memory device start at __address__.  This
handles any number for __byte_count__- it will break up the write into
multiple page writes as necessary.

nk_spiflash_write issues the write enable command first, so you do not have
to do it.

nk_spiflash_write waits until the write is complete before returning.

nk_spiflash_write does not erase the memory first- you have to call
nk_spiflash_erase first, if necessary.

Returns 0 for success, -1 for error.

## nk_spiflash_read

~~~c
int nk_spiflash_read(const void *info, /* const struct nk_spiflash_info *info, */ uint32_t address, uint8_t *data, uint32_t byte_count);
~~~

Read __byte_count__ bytes from the memory device starting at __address__. 
The read data is stored in array __data__.  __Address__ and __byte_count__
can be any values- the flash memory automatically crosses page boundaries.

Returns 0 for success, -1 for error.

## nk_spiflash_hex_dump

~~~c
void nk_spiflash_hex_dump(const struct nk_spiflash_info *info, uint32_t addr, uint32_t len);
~~~

Generate a hex dump of a region of the memory device and print it to
nkstdout.

Returns void, but prints error messages if there are SPI-transfer errors.

## nk_spiflash_crc

~~~c
uint32_t nk_spiflash_crc(const struct nk_spiflash_info *info, uint32_t addr, uint32_t len);
~~~

Compute 32-bit CRC of a region of the memory device.

## nk_spiflash_command

~~~c
int nk_spiflash_command(const struct nk_spiflash_info *info, nkinfile_t *args, uint32_t *old_addr);
~~~

Generic command line user interface to SPI-flash or EEPROM.  __args__ is
parsed for the following commands:

* rd nnn             Read a word
* wr nnn vvv         Write a word
* hd start size      Produce hex dump of region of memory
* crc start size     CRC of a region of memory
* erase start size   Erase a region of memory
* fill start size    Fill a region of memory with a test pattern
* fill start size vv Fill a region of memory with a byte
