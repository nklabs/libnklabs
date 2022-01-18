#include "nkdriver_max6675.h"

int nk_max6675_read_temp(const nkspi_device_t *dev, float *val)
{
    uint8_t buf[2];
    uint16_t v;
    int rtn = nk_spi_transfer(dev, buf, 2);
    v = (buf[0] << 8) + buf[1];
    *val = (float)(v >> 3) * .25f;
    return rtn;
}
