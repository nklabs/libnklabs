// MCP4725 12-bit I2C DAC

#include "nkdriver_mcp4725.h"

int nk_mcp4725_write(const nk_i2c_device_t *dev, uint16_t val)
{
    uint8_t data[2];
    val &= 0xFFF;
    data[0] = (val >> 8);
    data[1] = val;
    return nk_i2c_write(dev, 2, data);
}
