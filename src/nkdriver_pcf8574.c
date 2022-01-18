// PCF8574 I2C GPIO expander

// This simple device uses quasi-birdirection pins.  For each bit:
//   Write 1 for input with pull-up
//   Write 0 for output low

#include "nkdriver_pcf8574.h"

// Read GPIO byte

int pcf8574_read(const nk_i2c_device_t *dev, uint8_t *val)
{
    return nk_i2c_read(dev, 1, val);
}

// Write GPIO byte

int pcf8574_write(const nk_i2c_device_t *dev, uint8_t val)
{
    return nk_i2c_write(dev, 1, &val);
}
