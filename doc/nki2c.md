# I2C bus driver

This abstraction layer sits between drivers for specific I2C devices and the
MCU vendor's hardware I2C bus driver.  It allows I2C device drivers to
access devices which exist on buses other than the ones directly supported
by the MCU hardware- for example, you could have an I2C bus using bitbang
software utilities or one that exists on an FPGA that is accessible over a
SPI interface.

To use this driver, you provide an nk_i2c_bus_t filled in with pointers to
functions that perform the bus transactions.  The first argument (a void
pointer) which is passed to these functions when they are called is also
provided in i2c_ptr.

To use the MCU's hardware, use nk_hal_i2c_write, nk_hal_i2c_write_nostop and
nk_hal_i2c_read for the functions, and a vendor handle for i2c_ptr (for
example, this should be a (I2C_HandleTypeDef *) for STM32 or (struct
i2c_m_sync_desc *) for ATSAM.

Each I2C device should have an nk_i2c_device_t.  This should be filled in
with a pointer to the nk_i2c_bus_t and with the I2C address of this device.

## Files

[nki2c.h](../inc/nki2c.h), [nki2c.c](../src/nki2c.c),
[nki2c_atsam.c](../src/nki2c_atsam.c), [nki2c_stm.c](../src/nki2c_stm.c)

~~~c
// Generic access to a specific I2C bus

typedef struct {
    // I2C bus access functions
    int (*i2c_write)(void *i2c_ptr, uint8_t addr, size_t len, const uint8_t *buf);
    int (*i2c_write_nostop)(void *i2c_ptr, uint8_t addr, size_t len, const uint8_t *buf);
    int (*i2c_read)(void *i2c_ptr, uint8_t addr, size_t len, uint8_t *buf);

    // First argument for above functions
    void *i2c_ptr;
} nk_i2c_bus_t;

// Generic access to a specific I2C device

typedef struct {
    // The bus the device is on
    const nk_i2c_bus_t *i2c_bus;

    // The device's I2C address
    uint8_t i2c_addr;
} nk_i2c_device_t;
~~~

## Direct driver

These perform low level I2C transaction- they just call the function you
provide in the nk_i2c_bus_t.

### nk_i2c_write

~~~c
int nk_i2c_write(const nk_i2c_device_t *device, size_t len, const uint8_t *buf);
~~~

Write __len__ bytes from byte array __buf__ to the I2C device.

### nk_i2c_write_nostop

~~~c
int nk_i2c_write_nostop(const nk_i2c_device_t *device, size_t len, const uint8_t *buf);
~~~

This is the same as nk_i2c_write, except that the I2C stop condition isn't
transmitted.  Instead, a repeated start is issued on a subsequent
nk_i2c_read call.  Some MCUs do not support this function.  In this case,
nk_i2c_write_nostop is equivalent to nk_i2c_write.

### nk_i2c_read

~~~c
int nk_i2c_read(const nk_i2c_device_t *device, size_t len, uint8_t *buf);
~~~

Read __len__ bytes to byte array __buf__ from the I2C device.

## Register interface

These functions provide a register interface on top of the direct I2C
driver.  Most I2C devices appear as a sequence of registers, addressed with
a one or two byte register address.  Each register could be a single byte, a
big endian or little endian short (16-bit work) or a big endian or little
endian long (32-bit word).


~~~c
// Register interface on top of direct access

int nk_i2c_put_byte(const nk_i2c_device_t *dev, uint8_t ofst, uint8_t data);
int nk_i2c_get_byte(const nk_i2c_device_t *dev, uint8_t ofst, uint8_t *data);

int nk_i2c_put2_byte(const nk_i2c_device_t *dev, uint16_t ofst, uint8_t data);
int nk_i2c_get2_byte(const nk_i2c_device_t *dev, uint16_t ofst, uint8_t *data);

int nk_i2c_put_leshort(const nk_i2c_device_t *dev, uint8_t ofst, uint16_t data);
int nk_i2c_get_leshort(const nk_i2c_device_t *dev, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_leshort(const nk_i2c_device_t *dev, uint16_t ofst, uint16_t data);
int nk_i2c_get2_leshort(const nk_i2c_device_t *dev, uint16_t ofst, uint16_t *data);

int nk_i2c_put_beshort(const nk_i2c_device_t *dev, uint8_t ofst, uint16_t data);
int nk_i2c_get_beshort(const nk_i2c_device_t *dev, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_beshort(const nk_i2c_device_t *dev, uint16_t ofst, uint16_t data);
int nk_i2c_get2_beshort(const nk_i2c_device_t *dev, uint16_t ofst, uint16_t *data);

int nk_i2c_put2_le24(const nk_i2c_device_t *dev, uint16_t ofst, uint32_t data);
int nk_i2c_get2_le24(const nk_i2c_device_t *dev, uint16_t ofst, uint32_t *data);

int nk_i2c_put2_melong(const nk_i2c_device_t *dev, uint16_t ofst, uint32_t data);
~~~

key:

__port__ is the I2C device handle.

__addr__ is the 7-bit I2C device address.

__ofst__ is the regsiter address: 8-bits for the functions with put/get
in their names or 16-bits for the functions with put2/get2 in their names.

__data__ has the data to write to the regsiter or an address where data read
from the register should be returned.

The register size is indicated by the function name:

* byte for single byte
* leshort for little endian short
* beshort for big endia short

## nk_i2c_command

~~~c
int nk_i2c_command(const nk_i2c_bus_t *bus, nkinfile_t *args);
~~~

Generic command line user interface to an I2C bus.  __args__ is
parsed for the following commands:

* scan    Scan bus for all I2C devcies
* w aa bb bb ... [r aa nn]   Write bytes (bbs) to device aa, then read nn bytes from aa.
