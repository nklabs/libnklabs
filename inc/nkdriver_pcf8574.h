// PCF8574 I2C GPIO expander

#ifndef _Inkdriver_pcf8574
#define _Inkdriver_pcf8574

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

#include "nki2c.h"

#define PCF8574_ADDR 0x20 // I2C address of device: 0x20 - 0x27 depending on A0 - A2 inputs

// Read GPIO byte

int pcf8574_read(const nk_i2c_device_t *dev, uint8_t *val);

// Write GPIO byte

int pcf8574_write(const nk_i2c_device_t *dev, uint8_t val);

#endif
