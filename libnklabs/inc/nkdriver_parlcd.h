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

// Connect to HD44780 LCD using four bit parallel interface
// For example for OSEPP "LCD Keypad Shield"

// Probably pull-ups to 5V needed for this to work reliably
//  LCD is 5V device, MCU is 3.3v but has 5V tolerant inputs

#ifndef _Inkdriver_parlcd
#define _Inkdriver_parlcd

#include "nkdriver_hd44780.h"
#include "nkpin.h"

typedef struct
{
    // d0 - d3 not connected, use four-bit mode
    const nk_pin_t *d4; // Arduino D4
    const nk_pin_t *d5; // Arduino D5
    const nk_pin_t *d6; // Arduino D6
    const nk_pin_t *d7; // Arduino D7
    const nk_pin_t *e; // Arduino D9
    // nk_pin_t *rw; // rw is tied low, no reading
    const nk_pin_t *rs; // Arduino D8

    // Low to turn backlight off, open-drain for backlight on
    // (has 1K pullup to NPN base)
    const nk_pin_t *backlight; // Arduino D10

    // Arduino A0 is connected to the buttons, use ADC to read button
} nk_parlcd_t;

int nk_parlcd_init(const nk_parlcd_t *dev, bool backlight, bool f_bit, bool n_bit);

int nk_parlcd_backlight(const nk_parlcd_t *dev, bool on);

int nk_parlcd_send_command(const nk_parlcd_t *dev, uint8_t cmd);

int nk_parlcd_send_data(const nk_parlcd_t *dev, uint8_t *data, int len);

#endif
