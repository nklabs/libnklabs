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

// INA219 bidirectional I2C current/power/voltage monitor

#include "nkdriver_ina219.h"

// Set up INA219: just write the calibration register

int nk_ina219_init(const nk_i2c_device_t *dev)
{
    return nk_i2c_put_beshort(dev, INA219_REG_CAL, CAL);
}

// Read from INA219

int nk_ina219_read(const nk_i2c_device_t *dev, float *vbus_rtn, float *vshunt_rtn, float *power_rtn, float *current_rtn)
{
    int rtn;
    uint16_t vshunt = 0;
    uint16_t vbus = 0;
    uint16_t power = 0;
    uint16_t current = 0;
    rtn = nk_i2c_get_beshort(dev, INA219_REG_VSHUNT, &vshunt);
    rtn |= nk_i2c_get_beshort(dev, INA219_REG_VBUS, &vbus);
    rtn |= nk_i2c_get_beshort(dev, INA219_REG_POWER, &power);
    rtn |= nk_i2c_get_beshort(dev, INA219_REG_CURRENT, &current);

    *vshunt_rtn = (10e-6f * (float)(int16_t)vshunt); // LSB is 10uV for PGA = /8
    *vbus_rtn = (4e-3f * (float)(vbus >> 3)); // LSB is 4mV
    *current_rtn = CURRENT_LSB * (float)(int16_t)current;
    *power_rtn = POWER_LSB * (float)power;

    return rtn;
}
