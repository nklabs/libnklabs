// PCF8575 I2C GPIO expander

#ifndef _Inkdriver_pcf8575
#define _Inkdriver_pcf8575

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

// This is just like the PCF8574, except 16-bits instead of 8

#include "nki2c.h"

#define PCF8575_I2C_ADDR 0x20 // 0x20 - 0x27 depending on A0-A2 inputs

// Read GPIO short

int pcf8575_read(const nk_i2c_device_t *dev, uint16_t *val);

// Write GPIO short

int pcf8575_write(const nk_i2c_device_t *dev, uint16_t val);

#endif
