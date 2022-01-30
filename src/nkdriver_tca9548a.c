#include "nkdriver_tca9548a.h"

// Select which busses should be connected to the master:
// just write to the single byte register

int nk_tca9548a_set(const nk_i2c_device_t *dev, uint8_t val)
{
    return nk_i2c_write(dev, 1, &val);
}

int nk_tca9548a_get(const nk_i2c_device_t *dev, uint8_t *val)
{
    return nk_i2c_read(dev, 1, val);
}
