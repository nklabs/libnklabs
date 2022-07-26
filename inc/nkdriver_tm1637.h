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

#ifndef _Inkdriver_tm1637
#define _Inkdriver_tm1637

#include "nkpin.h"

#define TM1637_MODE_LOAD 0x40 // Set data mode to load digits
#define TM1637_MODE_READ 0x42 // Set data mode to read keyboard
#define TM1637_ADDR 0xC0 // Set starting write address to 0
#define TM1637_DISP_ON 0x88 // Display on, set brightness 0 - 7
#define TM1637_DISP_OFF 0x80 // Display off

typedef struct {
    const nk_pin_t *clk;
    const nk_pin_t *dio;

    // TM1637 boards I've seen have large capacitors on CLK and DIA,
    // so the speed is limited by the rise/fall times on these lines.

    // TM1637 itself is pretty fast, but the boards must have noise
    // problems.

    const int trise; // 200 us on one unit RC pull-up
    const int tfall; // 3 us on one unit C with max pin current pull-down

    // Segments seem to be connected consistently between units,
    // but digits are not.  Here is a mapping:

    uint8_t digit_map[6];
} nk_tm1637_t;

int nk_tm1637_init(const nk_tm1637_t *dev);

// Write 6 digits to TM1637
// This writes directly to the TM1637 refresh registers seguentially as is
// Bright should be 0 - 7
int nk_tm1637_display_raw(const nk_tm1637_t *dev, const uint8_t *digits);

// Use built-in font to convert ASCII to 7-segments:
// ASCII 0 - 9, A - F, a - F, space and - are converted
// Otherwise use lower four bits for hexadecimal
// Digits are remapped through digit_map, so the display order is up to you
// dp is a bitmap of which decimal points to illuminate (SEG8) 
// 'digits' should be 6 bytes
int nk_tm1637_display(const nk_tm1637_t *dev, uint8_t dp, const uint8_t *digits);

// Turn display on with brightness 0..7
int nk_tm1637_display_on(const nk_tm1637_t *dev, uint8_t brightness);

// Turn display off
int nk_tm1637_display_off(const nk_tm1637_t *dev);

// Scan keypad: returns a single 8-bit word bitmap
int nk_tm1637_keyscan(const nk_tm1637_t *dev, uint8_t *data);

// Scan codes: (SG1:K1 means switch between K1 and SEG1/KS1 is closed)
// SG1:K1	0xF7
// SG1:K2	0xEF
// SG2:K1	0xF6
// SG2:K2	0xEE
// SG3:K1	0xF5
// SG3:K2	0xED
// SG4:K1	0xF4
// SG4:K2	0xEC
// SG5:K1	0xF3
// SG5:K2	0xEB
// SG6:K1	0xF2
// SG6:K2	0xEA
// SG7:K1	0xF1
// SG7:K2	0xE9
// SG8:K1	0xF0
// SG8:K2	0xE8

#endif
