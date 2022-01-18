// MCP9808 temperature sensor

#include "nkdriver_mcp9808.h"

int nk_mcp9808_read_temp(const nk_i2c_device_t *dev, float *val)
{
    int rtn;
    uint16_t rdata = 0;
    rtn = nk_i2c_get_beshort(dev, 5, &rdata);

    if (rdata & 0x1000)
        rdata |= 0xE000;
    else
        rdata &= 0x1FFF;

    *val = (int16_t)rdata * 0.0625f;
    return rtn;
}
