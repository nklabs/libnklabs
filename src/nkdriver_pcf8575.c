// PCF8575 I2C GPIO expander

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

// This is just like the PCF8574, except 16-bits instead of 8

#include "nkdriver_pcf8575.h"

// Read GPIO short

int pcf8575_read(const nk_i2c_device_t *dev, uint16_t *val)
{
    return nk_i2c_read(dev, 2, (uint8_t *)val);
}

// Write GPIO short

int pcf8575_write(const nk_i2c_device_t *dev, uint16_t val)
{
    return nk_i2c_write(dev, 2, (uint8_t *)&val);
}
