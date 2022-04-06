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

// MAX7219 7-segment LED display driver

#ifndef _Inkdriver_max7219
#define _Inkdriver_max7219

#include "nkpin.h"

typedef struct {
    const nk_pin_t *mosi; // Write data
    const nk_pin_t *sclk; // Shift clock
    const nk_pin_t *load; // Latch on rising
    int trise; // 200 us on one unit RC pull-up
    int tfall; // 3 us on one unit C with max pin current pull-down
    uint8_t digit_map[8];
} nk_max7219_t;

#define MAX7219_CMD_NOP 0x0000
#define MAX7219_CMD_DIGIT0 0x0100
#define MAX7219_CMD_DIGIT1 0x0200
#define MAX7219_CMD_DIGIT2 0x0300
#define MAX7219_CMD_DIGIT3 0x0400
#define MAX7219_CMD_DIGIT4 0x0500
#define MAX7219_CMD_DIGIT5 0x0600
#define MAX7219_CMD_DIGIT6 0x0700
#define MAX7219_CMD_DIGIT7 0x0800

#define MAX7219_CMD_DECODE_MODE 0x0900 // Each set bit is a decoded digit
#define MAX7219_CMD_DECODE_MODE_NONE 0x0900 // Deocde no digits
#define MAX7219_CMD_DECODE_MODE_ALL 0x09FF // Decode all digits


#define MAX7219_CMD_INTENSITY 0x0A00 // Add 0x00 - 0x0F (0x0F is max)

#define MAX7219_CMD_SCAN_LIMIT 0x0B00 // Indicates last digit number to scan
#define MAX7219_CMD_SCAN_LIMIT_ALL 0x0B07 // Scan all 8 digits

#define MAX7219_CMD_SHUTDOWN 0x0C00
#define MAX7219_CMD_NO_SHUTDOWN 0x0C01

#define MAX7219_CMD_NO_TEST_MODE 0x0F00
#define MAX7219_CMD_TEST_MODE 0x0F01

int nk_max7219_init(const nk_max7219_t *dev);

// Write 8 digits to MAX7219
// This writes directly to the MAX7219 refresh registers seguentially as is
int nk_max7219_display_raw(const nk_max7219_t *dev, const uint8_t *digits);

// Use built-in font to convert ASCII to 7-segments:
// ASCII 0 - 9, A - F, a - F, space and - are converted
// Otherwise use lower four bits for hexadecimal
// Digits are remapped through digit_map, so the display order is up to you
// dp is a bitmap of which decimal points to illuminate (SEG8) 
// 'digits' should be 6 bytes
int nk_max7219_display(const nk_max7219_t *dev, uint8_t dp, const uint8_t *digits);

// Turn display on with brightness 0..F
int nk_max7219_display_on(const nk_max7219_t *dev, uint8_t brightness);

// Turn display off
int nk_max7219_display_off(const nk_max7219_t *dev);

#endif
