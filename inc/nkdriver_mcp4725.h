// MCP4725 12-bit I2C DAC

#ifndef _Inkdriver_mcp4725
#define _Inkdriver_mcp4725

#include "nki2c.h"

#define MCP4725_I2C_ADDR 0x60

int nk_mcp4725_write(const nk_i2c_device_t *dev, uint16_t val);

#endif
