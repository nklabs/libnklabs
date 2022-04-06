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

// Holtek HT16K33 LED driver
// Connected to four 14-digit LED display digits on Adafruit board

#ifndef _Inkdriver_ht16k33
#define _Inkdriver_ht16k33

#include "nki2c.h"

#define HT16K33_I2C_ADDR 0x70 // plus address on A2:A0 pins

#define HT16K33_CMD_DATA 0x00 // Set display RAM address, load display RAM
#define HT16K33_A_MASK 0x0F // Display RAM address mask

#define HT16K33_CMD_SETUP 0x20 // System setup
#define HT16K33_S_BIT 0x01 // Enable oscillator

#define HT16K33_CMD_KEY 0x40 // Set key RAM address, read key ram
#define HT16K33_KEY_MASK 0x07 // Key RAM address mask (0 - 5)

#define HT16K33_CMD_INT 0x60 // Read INT flag status

#define HT16K33_CMD_DISP_SETUP 0x80 // Display setup
#define HT16K33_D_BIT 0x01 // Display on
#define HT16K33_BLINK_OFF 0x00 // Blink control
#define HT16K33_BLINK_20 0x02
#define HT16K33_BLINK_10 0x04
#define HT16K33_BLINK_05 0x06

#define HT16K33_CMD_SETUP_ROW_PIN 0xA0 // Set up int/row pin
#define HT16K33_PIN_ROW 0x00
#define HT16K33_PIN_INT_LOW 0x01
#define HT16K33_PIN_INT_HIGH 0x03

#define HT16K33_CMD_DIMMING 0xE0 // Set dimming
#define HT16K33_DIM_MASK 0x0F // 0=1/16, F=16/16 duty cycle

int nk_ht16k33_init(const nk_i2c_device_t *dev);

// Write 6 digits to TM1637
// This writes directly to the TM1637 refresh registers seguentially as is
// Bright should be 0 - 7
int nk_ht16k33_display_raw(const nk_i2c_device_t *dev, const uint16_t *digits);

// Use built-in font to convert ASCII to 14-segments:
int nk_ht16k33_display(const nk_i2c_device_t *dev, uint8_t dp, const uint8_t *digits);

// Turn display on with brightness 0..7
int nk_ht16k33_display_on(const nk_i2c_device_t *dev, uint8_t brightness);

// Turn display off
int nk_ht16k33_display_off(const nk_i2c_device_t *dev);

// Scan keypad: returns a single 8-bit word bitmap
int nk_ht16k33_keyscan(const nk_i2c_device_t *dev, uint8_t *data);

#endif
