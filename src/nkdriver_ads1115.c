#include "nkdriver_ads1115.h"

int nk_ads1115_set_mode(const nk_i2c_device_t *dev, uint16_t val)
{
    return nk_i2c_put_beshort(dev, ADS1115_REG_CONFIG, val);
}

int nk_ads1115_set_thresh(const nk_i2c_device_t *dev, int16_t low, int16_t high)
{
    int rtn;
    rtn = nk_i2c_put_beshort(dev, ADS1115_REG_THRESH_LOW, (uint16_t)low);
    rtn |= nk_i2c_put_beshort(dev, ADS1115_REG_THRESH_HIGH, (uint16_t)high);
    return rtn;
}

int nk_ads1115_read(const nk_i2c_device_t *dev, int16_t *val)
{
    return nk_i2c_get_beshort(dev, ADS1115_REG_CONVERSION, (uint16_t *)val);
}

int nk_ads1115_get_mode(const nk_i2c_device_t *dev, uint16_t *val)
{
    return nk_i2c_get_beshort(dev, ADS1115_REG_CONFIG, (uint16_t *)val);
}
