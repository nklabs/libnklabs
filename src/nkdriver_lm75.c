#include "nkcli.h"
#include "nki2c.h"

int nk_lm75_read_temp(const nk_i2c_device_t *dev, float *val)
{
    int rtn;
    uint16_t rdata = 0;
    int16_t v;
    rtn = nk_i2c_get_beshort(dev, 0, &rdata);
    v = (int16_t)rdata;
    if (v < 0) {
        // Avoid signed right shift
        *val = -((-v >> 7)) * 0.5f;
    } else {
        *val = (v >> 7) * 0.5f;
    }
    return rtn;
}
