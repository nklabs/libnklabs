# I2C EEPROM Driver

This is a simple driver for most I2C EEPROM memory devices.

## Files

[nki2c_eeprom.h](../inc/nki2c_eeprom.h),
[nki2c_eeprom.c](../src/nki2c_eeprom.c)

## nk_i2c_eeprom_info structure

You initialize a structure containing information about the device as well
as pointers to functions which perform I2C transactions to the device.  The
address of this structure should be given as the first argument to the
provided memory access functions.

~~~c
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
~~~

__i2c_write__ is a pointer to a function which performs an I2C write
transaction on the I2C bus containing the memory device.  It should write
__len__ bytes to the device.  The write data is given in the array __buf__. 
__addr__ contains the 7-bit I2C address of the memory device.  __i2c_write__
should return 0 for success or anything else to indicate an error.

__i2c_read__ is a pointer to a function which performs an I2C read
transaction on the I2C bus containing the memory device.  It should read
__len__ bytes from the device.  The read data is returned in the array __buf__. 
__addr__ contains the 7-bit I2C address of the memory device.  __i2c_read__
should return 0 for success or anything else to indicate an error.

__buffer__ is the address of a memory buffer that can be used as the
transfer buffer for SPI transactions.  It will be used as the __buf__
argument for __i2c_write__ and __i2c_read__.  __buffer__ must be equal to or
larger than __page_size__ + __addr_size__.

__page_size__ should be a power of 2.  Memory reads and writes are broken up
into transactions no larger than __page_size__.  __page_size__ +
__addr_size__ + 1 must not be larger than the maximum transfer size allowed
by __i2c_write__.

__busy_timeout__ is the number of times status polling is performed when
waiting for writes to complete before timing out with an error.

__addr_size__ is the number of address bytes required by the memory device. 
It is in the range 1 to 2, depending on the memory size.

## nk_i2c_eeprom_busy_wait

~~~c
int nk_i2c_eeprom_busy_wait(const struct nk_i2c_eeprom_info *info);
~~~

Repeatedly poll the I2C memory device until it is no longer busy.  It does
this by writing a 0-length transaction to the device, checking if it
acknowledges it.  You do not normally have to call this function.

Returns 0 for success, -1 for timeout, or whatever __i2c_write__ returns for
other errors.

## nk_i2c_eeprom_write

~~~c
int nk_i2c_eeprom_write(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);
~~~

Write __byte_count__ bytes to the memory device starting at __address__.  This
handles any number for __byte_count__- it will break up the write into
multiple page writes as necessary.

nk_i2c_eeprom_write waits until the write is complete before returning.

Returns 0 for success, -1 for error.

## nk_i2c_eeprom_read

~~~c
int nk_i2c_eeprom_read(const struct nk_i2c_eeprom_info *info, uint32_t address, uint8_t *data, uint32_t byte_count);
~~~

Read __byte_count__ bytes from the memory device starting at __address__. 
The read data is stored in array __data__.  __Address__ and __byte_count__
can be any values- the flash memory automatically crosses page boundaries.

Returns 0 for success, -1 for error.

## nk_i2c_eeprom_hex_dump

~~~c
void nk_i2c_eeprom_hex_dump(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len);
~~~

Generate a hex dump of a region of the memory device and print it to
nkstdout.

Returns void, but prints error messages if there are I2C transaction errors.

## nk_i2c_eeprom_crc

~~~c
uint32_t nk_i2c_eeprom_crc(const struct nk_i2c_eeprom_info *info, uint32_t addr, uint32_t len);
~~~

Compute 32-bit CRC of a region of the memory device.

## nk_i2c_eeprom_command

~~~c
int nk_i2c_eeprom_command(const struct nk_i2c_eeprom_info *info, nkinfile_t *args, uint32_t *old_addr);
~~~

Generic command line user interface for an I2C EEPROM.  __args__ is
parsed for the following commands:

* rd nnn             Read a word
* wr nnn vvv         Write a word
* hd start size      Produce hex dump of region of memory
* crc start size     CRC of a region of memory
* erase start size   Erase a region of memory
* fill start size    Fill a region of memory with a test pattern
* fill start size vv Fill a region of memory with a byte
