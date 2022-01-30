#ifndef _Inkdriver_tca9548a
#define _Inkdriver_tca9548a

#include "nki2c.h"

// 8-channel I2C Switch
// 400 KHz

// There is just a single one-byte register:
// A bit is high to enable the corresponding channel.

#define TCA9548A_I2C_ADDR 0x70 // Add 0 - 7 depending on A2:A0 pins

// Select which busses should be connected to the master:

int nk_tca9548a_set(const nk_i2c_device_t *dev, uint8_t val);

// Read current switch setting

int nk_tca9548a_get(const nk_i2c_device_t *dev, uint8_t *val);

#endif
