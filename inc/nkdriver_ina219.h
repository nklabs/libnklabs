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

#ifndef _Inkdriver_ina219
#define _Inkdriver_ina219

#include "nki2c.h"

#define INA219_I2C_ADDR 0x40

#define INA219_REG_CONFIG 0x00
#define INA219_REG_VSHUNT 0x01
#define INA219_REG_VBUS 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CAL 0x05

#define RSHUNT 0.1f
#define CURRENT_MAX 3.2f // Amps
#define CURRENT_LSB (CURRENT_MAX / 32768.0f)
#define POWER_LSB (20.0f * CURRENT_LSB)
#define CAL (.04096f / (CURRENT_LSB * RSHUNT))

int nk_ina219_init(const nk_i2c_device_t *dev);

int nk_ina219_read(const nk_i2c_device_t *dev, float *vbus_rtn, float *vshunt_rtn, float *power_rtn, float *current_rtn);

#endif
