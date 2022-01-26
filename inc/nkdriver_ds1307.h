// Copyright 2020 NK Labs, LLC

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

#ifndef _Inkdriver_ds1307
#define _Inkdriver_ds1307

#include "nkdatetime.h"
#include "nki2c.h"

// DS1307 Real Time Clock
// 32 KHz output is open drain
// 100 KHz I2C Capable
// 5V

#define DS1307_I2C_ADDR 0x68

// DS1307 Registers
#define DS1307_REG_SECONDS 0x00 // BCD Seconds [00..59]

#define DS1307_REG_CH_BIT 0x80 // Clock halt when set.  Set on reset.

#define DS1307_REG_MINUTES 0x01 // BCD Minutes [00..59]
#define DS1307_REG_HOURS 0x02 // BCD Hours [00..23], Bit set for 12-hour mode
#define DS1307_REG_DAY 0x03 // Day of week [1..7]
#define DS1307_REG_DATE 0x04 // BCD  Day of month [1..31]
#define DS1307_REG_MONTH 0x05 // BCD Month [1..12]
#define DS1307_REG_YEAR 0x06 // BCD Year [00.99]
#define DS1307_REG_CTRL 0x07 // Control register

#define DS1307_REG_OUT_BIT 0x80 // Value of out pin when SQWE = 0
#define DS1307_REG_SQWE_BIT 0x10 // Enable SQWE

#define DS1307_REG_RS_1HZ 0x00 // 1 Hz
#define DS1307_REG_RS_4KHZ 0x01 // 4096 Hz
#define DS1307_REG_RS_8KHZ 0x02 // 8192 Hz
#define DS1307_REG_RS_32KHZ 0x03 // 32768 Hz

#define DS1307_REG_RAM 0x08 // Ram from here to 0x3F

int nk_ds1307_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime);
int nk_ds1307_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime);

#endif
