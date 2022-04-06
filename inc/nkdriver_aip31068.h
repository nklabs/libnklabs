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

#ifndef _Inkdriver_aip31068
#define _Inkdriver_aip31068

#include "nki2c.h"
#include "nkdriver_hd44780.h"

#define AIP31068_I2C_ADDR 0x3E

// Control byte format
#define AIP31068_CO 0x80 // 1 = allow another control byte, 0 = this is last one
#define AIP31068_RS 0x40 // 1 = write data, 0 = write command

// Initialize AIP31068

int nk_aip31068_init(const nk_i2c_device_t *dev, bool f_bit, bool n_bit);

// Send a command byte

int nk_aip31068_send_command(const nk_i2c_device_t *dev, uint8_t cmd);

// Send data bytes

int nk_aip31068_send_data(const nk_i2c_device_t *dev, uint8_t *data, int len);

#endif
