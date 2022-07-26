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

#ifndef _Inki2c
#define _Inki2c

#include <stdint.h>
#include "nkcli.h"
#include "nkpin.h"

// Bitbang I2C

typedef struct {
    nk_pin_t *scl;
    nk_pin_t *sda;
} nk_bitbang_i2c_bus_t;

int nk_bitbang_i2c_write(nk_bitbang_i2c_bus_t *bus, uint8_t addr, size_t len, const uint8_t *buf);
int nk_bitbang_i2c_write_nostop(nk_bitbang_i2c_bus_t *bus, uint8_t addr, size_t len, const uint8_t *buf);
int nk_bitbang_i2c_read(nk_bitbang_i2c_bus_t *bus, uint8_t addr, size_t len, uint8_t *buf);

// Functions to use for MCU's I2C hardware

int nk_hal_i2c_write(void *port, uint8_t addr, size_t len, const uint8_t *buf);
int nk_hal_i2c_write_nostop(void *port, uint8_t addr, size_t len, const uint8_t *buf);
int nk_hal_i2c_read(void *port, uint8_t addr, size_t len, uint8_t *buf);

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

// Direct I2C access

int nk_i2c_write(const nk_i2c_device_t *device, size_t len, const uint8_t *buf);
int nk_i2c_write_nostop(const nk_i2c_device_t *device, size_t len, const uint8_t *buf);
int nk_i2c_read(const nk_i2c_device_t *device, size_t len, uint8_t *buf);

// Register interface on top of direct access

int nk_i2c_put_byte(const nk_i2c_device_t *device, uint8_t ofst, uint8_t data);
int nk_i2c_get_byte(const nk_i2c_device_t *device, uint8_t ofst, uint8_t *data);

int nk_i2c_put2_byte(const nk_i2c_device_t *device, uint16_t ofst, uint8_t data);
int nk_i2c_get2_byte(const nk_i2c_device_t *device, uint16_t ofst, uint8_t *data);

int nk_i2c_put_leshort(const nk_i2c_device_t *device, uint8_t ofst, uint16_t data);
int nk_i2c_get_leshort(const nk_i2c_device_t *device, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_leshort(const nk_i2c_device_t *device, uint16_t ofst, uint16_t data);
int nk_i2c_get2_leshort(const nk_i2c_device_t *device, uint16_t ofst, uint16_t *data);

int nk_i2c_put_beshort(const nk_i2c_device_t *device, uint8_t ofst, uint16_t data);
int nk_i2c_get_beshort(const nk_i2c_device_t *device, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_beshort(const nk_i2c_device_t *device, uint16_t ofst, uint16_t data);
int nk_i2c_get2_beshort(const nk_i2c_device_t *device, uint16_t ofst, uint16_t *data);

int nk_i2c_put2_le24(const nk_i2c_device_t *device, uint16_t ofst, uint32_t data);
int nk_i2c_get2_le24(const nk_i2c_device_t *device, uint16_t ofst, uint32_t *data);

int nk_i2c_put2_melong(const nk_i2c_device_t *device, uint16_t ofst, uint32_t data);

// Common CLI for an I2C bus

int nk_i2c_command(const nk_i2c_bus_t *bus, nkinfile_t *args);

#endif
