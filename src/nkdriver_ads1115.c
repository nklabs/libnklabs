// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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
