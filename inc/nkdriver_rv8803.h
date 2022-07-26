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

#ifndef _Inkdriver_rv8803
#define _Inkdriver_rv8803

#include "nkdatetime.h"
#include "nki2c.h"

// RV-8803-C7 Real Time Clock driver

// Notes: You must read multiple time and compare the result to ensure consistency (the
// chip doesn't do it automatically).

// Older versions of this chip had I2C bugs, check errata:
//   Need to retry if you get erroneous NAK
//   Stop condition not recognized on writes, you must always do a final read

// 400 KHz I2C Capable

#define RV8803_I2C_ADDR 0x32

// RV-8803 Registers

#define RV8803_REG_SECONDS 0x00 // 00 - 59, BCD
#define RV8803_REG_MINUTES 0x01 // 00 - 59, BCD
#define RV8803_REG_HOURS 0x02 // 00 - 23, BCD
#define RV8803_REG_DAY 0x03 // One hot coded: 0x01 = Sunday, 0x02 = Monday, etc.
#define RV8803_REG_DATE 0x04 // 01 - 31, BCD
#define RV8803_REG_MONTH 0x05 // 01 - 12, BCD
#define RV8803_REG_YEAR 0x06 // 00 - 99, BCD

#define RV8803_REG_RAM 0x07 // Free RAM, reset to 0

#define RV8803_REG_AL1_MINUTES 0x08 // 00 - 59, BCD.  Bit 7 is minute enable
#define RV8803_REG_AL1_HOURS 0x09 // 00 - 23, BCD. Bit 6 is free RAM, bit 7 is hour enable
#define RV8803_REG_AL1_DAY 0x0A // WADA bit = 0: Weekday one hot coded, bit 7 is weekday enable
                                // WADA bit = 1: Day 01 - 31, BCD, bit 7 is free RAM, bit 7 is date enable
#define RV8803_REG_TIMER0 0x0B // Write low half of countdown timer
#define RV8803_REG_TIMER1 0x0C // Write upper four bits of countdown timer (in lower 4 bits of this reg), Upper 4 bits free RAM

#define RV8803_REG_EXTENSION 0x0D

// Extension register bits:

#define RV8803_TEST_BIT 0x80 // Always zero
#define RV8803_WADA_BIT 0x40 // Default is 0
#define RV8803_USEL_BIT 0x20 // Default is 0
#define RV8803_TE_BIT 0x10 // Defualt is 0

// CLKOUT frequency
#define RV8803_FD32KHZ 0x00 // Default
#define RV8803_FD1KHZ 0x04
#define RV8803_FD1HZ 0x08

// Timer clock selection
#define RV8803_TD4096HZ 0x00 // Default
#define RV8803_TD64HZ 0x01
#define RV8803_TD1HZ 0x02
#define RV8803_TD1PM 0x03 // 1 / 60 Hz, Once Per Minute

#define RV8803_REG_FLAGS 0x0E

#define RV8803_UF_BIT 0x20
#define RV8803_TF_BIT 0x10
#define RV8803_AF_BIT 0x08
#define RV8803_EVF_BIT 0x04
#define RV8803_V2F_BIT 0x02
#define RV8803_V1F_BIT 0x01

#define RV8803_REG_CONTROL 0x0F

#define RV8803_UIE_BIT 0x20
#define RV8803_TIE_BIT 0x10
#define RV8803_AIE_BIT 0x08
#define RV8803_EIE_BIT 0x04
#define RV8803_RESET_BIT 0x01

int nk_rv8803_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime);
int nk_rv8803_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime);

#endif
