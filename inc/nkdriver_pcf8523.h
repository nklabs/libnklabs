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

#ifndef _Inkdriver_pcf8523
#define _Inkdriver_pcf8523

#include "nki2c.h"
#include "nkdatetime.h"

// PCF8523 Real Time Clock

// 1 MHz I2C Capable

#define PCF8523_I2C_ADDR 0x68

// PCF8523 Registers
#define PCF8523_REG_CONTROL_1 0x00

#define PCF8523_CIE_BIT 0x01 // 1 = correction cycle interrupts, 0 = disabled (default)
#define PCF8523_AIE_BIT 0x02 // 1 = alarm interrupt enabled, 0 = disabled (default)
#define PCF8523_SIE_BIT 0x04 // 1 = seconds interrupt enabled, 0 = disabled (default)
#define PCF8523_12_BIT 0x08 // 1 = 12 hour mode, 0 = 24 hour mode (default)
#define PCF8523_SR_BIT 0x10 // 1 = software reset, 0 = no reset (default)
#define PCF8523_STOP_BIT 0x20 // 1 = RTC frozen, 0 = RTC running (default)
#define PCF8523_CAP_SEL_BIT 0x80 // 1 = 12.5 pF, 0 = 7pF (default)

// For a software reset, write 0x58 to CONTROL_1

#define PCF8523_REG_CONTROL_2 0x01

#define PCF8523_CTBIE_BIT 0x01 // 1 = Countdown timer B interrupt enable, 0 = disabled (default)
#define PCF8523_CTAIE_BIT 0x02 // 1 = Countdown timer A interrupt enabled, 0 = disable (default)
#define PCF8523_WTAIE_BIT 0x04 // 1 = Watchdog timer interrupt enabled, 0 = disabled (default)
#define PCF8523_AF_BIT 0x08 // 1 indicates alarm tiggered, software must clear
#define PCF8523_SF_BIT 0x10 // 1 indicates seconds int triggered, software clears
#define PCF8523_CTBF_BIT 0x20 // 1 indicates countdown timer B triggered, software clears
#define PCF8523_CTAF_BIT 0x40 // 1 indicates countdown timer A triggered, software clears
#define PCF8523_WTAF_BIT 0x80 // 1 indicates watchdog timer triggered, software clears

#define PCF8523_REG_CONTROL_3 0x02

#define PCF8523_BLIE_BIT 0x01 // 1 enables interrupt from BLF (0 is default)
#define PCF8523_BSIE_BIT 0x02 // 1 enables interrupt from BSF (0 is default)
#define PCF8523_BLF_BIT 0x04 // 1 = battery low
#define PCF8523_BSF_BIT 0x08 // 1 = battery switchover, software clears

#define PCF8523_PM_SHIFT 5 // battery switch-over and low detection control see below
#define PCF8523_PM_MASK 0x07
// 000 standard switchover mode, low detection enabled
// 001 direct switchover mode, low detection enabled
// 010, 011 switchover disabled, low detection enabled
// 100 standard switchover mode, low detection disabled
// 101 direct switchover mode, low detection disabled
// 110 not allowed

// 111 switchover disabled, low detection disabled (default)
//     battery will not power clock, only VCDD

// Standard mode: VDD < VBAT and VDD < 2.5V
// Direct mode: VDD < VBAT

#define PCF8523_REG_SECONDS 0x03 // BCD Seconds [00..59], bit 7 is OS bit

#define PCF8523_OS_BIT 0x80 // Oscillator stopped bit

#define PCF8523_REG_MINUTES 0x04 // BCD Minutes [00..59]
#define PCF8523_REG_HOURS 0x05 // BCD Hours [00..23]
#define PCF8523_REG_DATE 0x06 // BCD Day [1..31]
#define PCF8523_REG_WEEKDAY 0x07 // Day of week [0..6]
#define PCF8523_REG_MONTH 0x08 // BCD Month [1..12], Bit 7 is century
#define PCF8523_REG_YEAR 0x09 // BCD Year [00.99]

#define PCF8523_REG_AL_MINUTES 0x0A // BCD Minutes [00..59], Bit 7 = AEN_M

#define PCF8523_AEN_M_BIT 0x80

#define PCF8523_REG_AL_HOURS 0x0B // BCD Hours [00..23], Bit 7 = AEN_H

#define PCF8523_AEN_H_BIT 0x80

#define PCF8523_REG_AL_DATE 0x0C // BCD Date [01..31], Bit 7 = AEN_D

#define PCF8523_AEN_D_BIT 0x80

#define PCF8523_REG_AL_WEEKDAY 0x0D // Day of week [0..6], Bit 7 = AEN_W

#define PCF8523_AEN_W_BIT 0x80

#define PCF8523_REG_OFFSET 0x0E // Bit 7 = Mode, Bit 6:0 = Offset

#define PCF8523_MODE_BIT 0x80 // 1 = offset corrections applied once per second, 0 = per minute

#define PCF8523_REG_CLKOUT 0x0F

#define PCF8523_TBC_BIT 0x01 // 1 = Timer B enabled, 0 = disabled

#define PCF8523_TAC_SHIFT 1
#define PCF8523_TAC_MASK 0x03
// 00 disabled (default)
// 01 timer A is countdown
// 10 timer A is watchdog
// 11 disabled


#define PCF8523_COF_SHIFT 3 // CLKOUT frequency selection
#define PCF8523_COF_MASK 0x07
// 0 = 32768 (default)
// 1 = 16384
// 2 = 8192
// 3 = 4096
// 4 = 1024
// 5 = 32
// 6 = 1
// 7 = CLKOUT disabled


#define PCF8523_TBM_BIT 0x40 // 1 = Pulsed interrupt timer B, 0 = non-pulsed (default)
#define PCF8523_TAM_BIT 0x80 // 1 = Pulsed interrupt timer A and second, 0 = non-pulsed (default)

#define PCF8523_REG_TMRA_FREQ 0x10

#define PCF8523_TAQ_SHIFT 0
#define PCF8523_TAQ_MASK 0x07
// Timer A source frequency
// 0 = 4096
// 1 = 64 Hz
// 2 = 1 Hz
// 3 1/60 Hz
// 4, 5, 7: 1/3600 Hz (7 is default)

#define PCF8523_REG_TMRA 0x11

#define PCF8523_REG_TMRB_FREQ 0x12

#define PCF8523_TBQ_SHIFT 0
#define PCF8523_TBQ_MASK 0x07
// 0 4096 Hz
// 1 64 Hz
// 2 1 Hz
// 3 1/60 Hz
// 4, 6, 7 (default): 1/3600 Hz

#define PCF8523_TBW_SHIFT 4
#define PCF8523_TBW_MASK 0x07
// 0 46.875 ms
// 1 62.5 ms
// 2 78.125 ms
// 3 93.75 ms
// 4 125 ms
// 5 156.24 ms
// 6 187.5 ms
// 7 218.75 ms

#define PCF8523_REG_TMRB 0x13

int nk_pcf8523_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime);

int nk_pcf8523_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime);

#endif
