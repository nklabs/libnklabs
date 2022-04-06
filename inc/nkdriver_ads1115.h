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

// ADS1115 16-bit A/D converter

#ifndef _Inkdriver_ads1115
#define _Inkdriver_ads1115

#include "nki2c.h"

#define ADS1115_I2C_ADDR 0x48

#define ADS1115_REG_CONVERSION 0 // 16-bit signed

#define ADS1115_REG_CONFIG 1

// Config reg bits: select one from each group and bit-wise OR them together

#define ADS1115_COMP_QUE_ONE 0x0000 // One conversion needed before Alert assertion
#define ADS1115_COMP_QUE_TWO 0x0001 // Two conversions needed before assertion
#define ADS1115_COMP_QUE_FOUR 0x0002 // Four conversions needed before assertion
#define ADS1115_COMP_QUE_DISABLE 0x0003 // Alert/RDY pin at high state

#define ADS1115_COMP_LAT 0x0004 // Set for latching Alert

#define ADS1115_COMP_POL 0x0008 // Set for active high Alert

#define ADS1115_COMP_MODE 0x0010 // Set for window comparator, clear for traditional comparator with hysteresis
// Traditional: assert Alert when input above thresh_high, then de-assert when input below thresh_low
// Window: assert Alert when input above thresh_high or below thresh_low

// Select data rate in samples per second
#define ADS1115_DATA_RATE_8 0x0000
#define ADS1115_DATA_RATE_16 0x0020
#define ADS1115_DATA_RATE_32 0x0040
#define ADS1115_DATA_RATE_64 0x0060
#define ADS1115_DATA_RATE_128 0x0080
#define ADS1115_DATA_RATE_250 0x00A0
#define ADS1115_DATA_RATE_475 0x00C0
#define ADS1115_DATA_RATE_860 0x00E0

#define ADS1115_SINGLE_SHOT 0x0100 // Single shot mode (clear for continuous)

// Select gain.  2048 means input range is +2.048 to -2.048 volts
// (No matter the gain here, input pins must be between VCC and GND).
#define ADS1115_PGA_6144 0x0000
#define ADS1115_PGA_4096 0x0200
#define ADS1115_PGA_2048 0x0400
#define ADS1115_PGA_1024 0x0600
#define ADS1115_PGA_512 0x0800
#define ADS1115_PGA_256 0x0A00

// Input MUX
#define ADS1115_MUX_AIN0_AIN1 0x0000 // Differential input select
#define ADS1115_MUX_AIN0_AIN3 0x1000
#define ADS1115_MUX_AIN1_AIN3 0x2000
#define ADS1115_MUX_AIN2_AIN3 0x3000
#define ADS1115_MUX_AIN0 0x4000 // Single ended input select
#define ADS1115_MUX_AIN1 0x5000
#define ADS1115_MUX_AIN2 0x6000
#define ADS1115_MUX_AIN3 0x7000

// Conversion status
#define ADS1115_OS 0x8000 // Write 1 to start a conversion, read: 0 indicates busy, 1 indicates done

#define ADS1115_REG_THRESH_LOW 2 // 16-bit signed

#define ADS1115_REG_THRESH_HIGH 3 // 16-bit signed

// Set up device
int nk_ads1115_set_mode(const nk_i2c_device_t *dev, uint16_t val);
int nk_ads1115_set_thresh(const nk_i2c_device_t *dev, int16_t low, int16_t high);

// Read latest sample
int nk_ads1115_read(const nk_i2c_device_t *dev, int16_t *val);

// Read status: this just reads the mode register
//  Bit-wise and result with ADS1115_OS to read done status
int nk_ads1115_get_mode(const nk_i2c_device_t *dev, uint16_t *val);

#endif
