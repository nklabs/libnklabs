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

#ifndef _Inkdriver_ds3231
#define _Inkdriver_ds3231

#include "nkdatetime.h"
#include "nki2c.h"

// DS3231 Real Time Clock with TCXO

// 32 KHz output is open drain
// INT_L / SQW output is open drain
// RST_L input has built-in pull-up

// 400 KHz I2C Capable

#define DS3231_I2C_ADDR 0x68

// DS3231 Registers
#define DS3231_REG_SECONDS 0x00 // BCD Seconds [00..59]
#define DS3231_REG_MINUTES 0x01 // BCD Minutes [00..59]
#define DS3231_REG_HOURS 0x02 // BCD Hours [00..23], Bit set for 12-hour mode
#define DS3231_REG_DAY 0x03 // Day of week [1..7]
#define DS3231_REG_DATE 0x04 // BCD  Day of month [1..31]
#define DS3231_REG_MONTH 0x05 // BCD Month [1..12], Bit 7 is century
#define DS3231_REG_YEAR 0x06 // BCD Year [00.99]

#define DS3231_REG_AL1_SECONDS 0x07 // BCD Seconds, Bit 7 = A1M1
#define DS3231_REG_AL1_MINUTES 0x08 // BCD Minutes, Bit 7 = A1M2
#define DS3231_REG_AL1_HOUR 0x09 // BCD Hours, Bit 6 = 12 hour, Bit 7 = A1M3
#define DS3231_REG_AL1_DAY 0x0A // BCD Day or Date, Bit 6 = Day, Bit 7 = A1M4

// A1M bits:
//  1111 Once per second
//  1110 When seconds match
//  1100 When minutes and seconds match
//  1000 When hours, minutes and seconds match
//  0000 When day/date, hours, minutes and seconds match

#define DS3231_REG_AL2_MINUTES 0x0B // BCD Mnutes, Bit 7 = A2M2
#define DS3231_REG_AL2_HOUR 0x0C // BCD Hours, Bit 6 = 12 hour, Bit 7 = A2M3
#define DS3231_REG_AL2_DAY 0x0D // BCD Day or DAte, Bit 6 = Day, Bit 7 = A2M4

// A2M bits:
//  111 Once per minute (seconds == 00)
//  110 When minutes match and seconds == 00
//  100 When hours, minutes match and seconds == 00
//  000 When day/date, hours, minutes match and seconds == 00

#define DS3231_REG_CONTROL 0x0E

#define DS3231_REG_CONTROL_OSC_EN_L_BIT 0x80 // Oscillator enabled on battery power when low (default = 0)
#define DS3231_REG_CONTROL_BBSQW_BIT 0x40 // Disable square wave on battery power when low (default = 0)
#define DS3231_REG_CONTROL_CONV_BIT 0x20 // Start temp conversion (otherwise they happen once every 64 seconds)
#define DS3231_REG_CONTROL_RS2_BIT 0x10 // 11 = 8 KHz, 10 = 4 KHz, 01 = 1 KHz, 00 = 1 Hz (default = 11)
#define DS3231_REG_CONTROL_RS1_BIT 0x08
#define DS3231_REG_CONTROL_INTCN_BIT 0x04 // 1 = Enable interrupt output, 0 = square wave (default = 1)
#define DS3231_REG_CONTROL_A2IE_BIT 0x02 // AL2 int enable on square wave pin, default = 0
#define DS3231_REG_CONTROL_A1IE_BIT 0x01 // AL1 int enable on square wave pin, default = 0

#define DS3231_REG_STATUS 0x0F

#define DS3231_REG_STATUS_OSF_BIT 0x80 // Clock stopped status (default = 1)
#define DS3231_REG_STATUS_EN32KHZ_BIT 0x08 // 1 means 32 KHz output is on, 0 = High-Z (default = 1)
#define DS3231_REG_STATUS_BSY_BIT 0x04 // 1 means busy with TCXO functions
#define DS3231_REG_STATUS_A2F_BIT 0x02 // 1 means alarm2 pending (write 0 to clear)
#define DS3231_REG_STATUS_A1F_BIT 0x01 // 1 means alarm1 pending (write 0 to clear)

#define DS3231_REG_AGING_OFFSET 0x10 // Higher is slower (default = 0) .1 PPM at 25C
#define DS3231_REG_TEMP_HIGH 0x11 // Upper 8 bits.  10 bit signed temperature, LSB = .25 C
#define DS3231_REG_TEMP_LOW 0x12 // Lower 2 bits in MSBs (Lower 6 bits unused)

int nk_ds3231_set_datetime(const nk_i2c_device_t *dev, const nkdatetime_t *datetime);
int nk_ds3231_get_datetime(const nk_i2c_device_t *dev, nkdatetime_t *datetime);

// Read temperature from DS3231
int nk_ds3231_get_temp(const nk_i2c_device_t *dev, float *temp);

#endif
