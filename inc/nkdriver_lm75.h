#ifndef _Inkdriver_lm75
#define _Inkdriver_lm75

#include "nki2c.h"

#define LM75_I2C_ADDR 0x48

// Return temperature in Celsius
int nk_lm75_read_temp(const nk_i2c_device_t *dev, float *val);

#endif
