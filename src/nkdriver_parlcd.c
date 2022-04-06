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

// Parallel HD44780-style LCD interface in 4-bit mode

#include "nkdriver_parlcd.h"

// Single nibble write

int nk_parlcd_write(const nk_parlcd_t *dev, int rs, uint8_t val)
{
    int rtn;

    // Data and register select
    rtn = nk_pin_write(dev->rs, !!rs);
    rtn |= nk_pin_write(dev->d4, !!(val & 0x10));
    rtn |= nk_pin_write(dev->d5, !!(val & 0x20));
    rtn |= nk_pin_write(dev->d6, !!(val & 0x40));
    rtn |= nk_pin_write(dev->d7, !!(val & 0x80));

    // Pulse E
    rtn |= nk_pin_write(dev->e, 1);
    nk_udelay(1);
    rtn |= nk_pin_write(dev->e, 0);
    nk_udelay(1);

    return rtn;
}

int nk_parlcd_init(const nk_parlcd_t *dev, bool backlight_on, bool f_bit, bool n_bit)
{
    int rtn;
    rtn = nk_pin_setmode(dev->d4, NK_PINMODE_OUTPUT);
    rtn |= nk_pin_setmode(dev->d5, NK_PINMODE_OUTPUT);
    rtn |= nk_pin_setmode(dev->d6, NK_PINMODE_OUTPUT);
    rtn |= nk_pin_setmode(dev->d7, NK_PINMODE_OUTPUT);
    rtn |= nk_pin_setmode(dev->e, NK_PINMODE_OUTPUT);
    rtn |= nk_pin_write(dev->e, 0);
    rtn |= nk_pin_setmode(dev->rs, NK_PINMODE_OUTPUT);

    // Backlight on
    rtn |= nk_parlcd_backlight(dev, backlight_on);

    // Reset sequence to put LCD into 8-bit mode
    nk_udelay(15000); // 15 ms for display to boot
    rtn |= nk_parlcd_write(dev, 0, HD44780_FUNC | HD44780_FUNC_DL);
    nk_udelay(4100);
    rtn |= nk_parlcd_write(dev, 0, HD44780_FUNC | HD44780_FUNC_DL);
    nk_udelay(61);
    rtn |= nk_parlcd_write(dev, 0, HD44780_FUNC | HD44780_FUNC_DL);

    // Switch to 4-bit mode
    rtn |= nk_parlcd_write(dev, 0, HD44780_FUNC);

    rtn |= nk_parlcd_send_command(dev, HD44780_FUNC | (f_bit ? HD44780_FUNC_F : 0) | (n_bit ? HD44780_FUNC_N : 0));

    // Display off, cursor off, blink off
    rtn |= nk_parlcd_send_command(dev, HD44780_DISPLAY);

    // Clear screen, home cursor
    rtn |= nk_parlcd_send_command(dev, HD44780_CLS);
    nk_udelay(1530);

    // Display on, cursor on, blink on
    rtn |= nk_parlcd_send_command(dev, HD44780_DISPLAY | HD44780_DISPLAY_D | HD44780_DISPLAY_C | HD44780_DISPLAY_B);

    return rtn;
}

int nk_parlcd_backlight(const nk_parlcd_t *dev, bool on)
{
    int rtn;
    if (on)
    {
        rtn = nk_pin_setmode(dev->backlight, NK_PINMODE_INPUT);
        rtn |= nk_pin_write(dev->backlight, 1);
    }
    else
    {
        rtn = nk_pin_setmode(dev->backlight, NK_PINMODE_OUTPUT_OD);
        rtn |= nk_pin_write(dev->backlight, 0);
    }
    return rtn;
}

int nk_parlcd_send_command(const nk_parlcd_t *dev, uint8_t cmd)
{
    int rtn = 0;
    rtn |= nk_parlcd_write(dev, 0, cmd);
    rtn |= nk_parlcd_write(dev, 0, cmd << 4);
    nk_udelay(39); // Min. delay before checking busy is 80us.  Most commands require 39us to complete.
    return rtn;
}

int nk_parlcd_send_data(const nk_parlcd_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    while (len--)
    {
        rtn |= nk_parlcd_write(dev, 1, *data);
        rtn |= nk_parlcd_write(dev, 1, *data << 4);
        nk_udelay(39); // Min. delay before checking busy is 80us.  Most commands require 39us to complete.
        ++data;
    }
    return rtn;
}
