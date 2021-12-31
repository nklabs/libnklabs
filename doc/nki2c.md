# I2C device driver

## Direct driver

These functions are simple wrappers around the MCU HAL's I2C device driver.

### nk_i2c_write

~~~c
int nk_i2c_write(void *port, uint8_t addr, size_t len, const uint8_t *buf);
~~~

Write __len__ bytes from byte array __buf__ to the I2C device with 7-bit
address __addr__.  __port__ has the handle to the I2C bus.

### nk_i2c_write_nostop

~~~c
int nk_i2c_write_nostop(void *port, uint8_t addr, size_t len, const uint8_t *buf);
~~~

This is the same as nk_i2c_write, except that the I2C stop condition isn't
transmitted.  Instead, a repeated start is issued on a subsequent
nk_i2c_read call.  Some MCUs do not support this function.  In this case,
nk_i2c_write_nostop is equivalent to nk_i2c_write.

### nk_i2c_read

~~~c
int nk_i2c_read(void *port, uint8_t addr, size_t len, uint8_t *buf);
~~~

Read __len__ bytes to byte array __buf__ from the I2C device with 7-bit
address __addr__.  __port__ has the handle to the I2C bus.

## Bitbang driver

If the bitbang I2C driver is used, then you must provide the following
functions:

~~~c
void nk_i2c_scl_low(void *port); // Drive SCL line low
void nk_i2c_scl_high(void *port); // Release SCL line
int nk_i2c_scl_peek(void *port); // Read SCL line

void nk_i2c_sda_low(void *port); // Drive SDA line low
void nk_i2c_sda_high(void *port); // Release SDA line
int nk_i2c_sda_peek(void *port); // Read SDA line
~~~

## Register interface

These functions provide a register interface on top of the direct I2C
driver.  Most I2C devices appear as a sequence of registers, addressed with
a one or two byte register address.  Each register could be a single byte, a
big endian or little endian short (16-bit work) or a big endian or little
endian long (32-bit word).


~~~c
// Register interface on top of direct access

int nk_i2c_put_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t data);
int nk_i2c_get_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t *data);

int nk_i2c_put2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t data);
int nk_i2c_get2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t *data);

int nk_i2c_put_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data);
int nk_i2c_get_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data);
int nk_i2c_get2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data);

int nk_i2c_put_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data);
int nk_i2c_get_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data);
int nk_i2c_get2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data);

int nk_i2c_put2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t data);
int nk_i2c_get2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t *data);

int nk_i2c_put2_melong(void *port, uint8_t addr, uint16_t ofst, uint32_t data);
~~~

key:

__port__ is the I2C device handle.

__addr__ is the 7-bit I2C device address.

__ofst__ is the regsiter address: 8-bits for the functions with put/get
in their names or 16-bits for the functions put2/get2 in their names.

__data__ has the data to write to the regsiter or an address where data read
from the register should be returned.

The register size is indicated by the function name:

* byte for single byte
* leshort for little endian short
* beshort for big endia short

## nk_i2c_command

~~~c
int nk_i2c_command(void *port, nkinfile_t *args);
~~~

Generic command line user interface to an I2C bus.  __args__ is
parsed for the following commands:

* scan    Scan bus for all I2C devcies
* w aa bb bb ... [r aa nn]   Write bytes (bbs) to device aa, then read nn bytes from aa.
