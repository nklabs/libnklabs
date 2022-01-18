// MCP9808 temperature sensor

#ifndef _Inkdriver_mcp9808
#define _Inkdriver_mcp9808

#include "nki2c.h"

#define MCP9808_I2C_ADDR 0x18

int nk_mcp9808_read_temp(const nk_i2c_device_t *dev, float *val);

#endif
