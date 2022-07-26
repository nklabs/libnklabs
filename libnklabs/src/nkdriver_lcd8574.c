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

#include "nkdriver_lcd8574.h"

// Generate single E pulse

int nk_lcd8574_pulse(nk_lcd8574_t *dev)
{
    int rtn = 0;
    rtn |= pcf8574_write(dev->pcf8574, dev->regval | LCD8574_E);
    // nk_udelay(1); // Min. pulse width is 450 ns
    rtn |= pcf8574_write(dev->pcf8574, dev->regval);
    // nk_udelay(39); // Min. delay before checking busy is 80us.  Most commands require 39us to complete.
    // Not needed because I2C at 100 KHz is longer
    return rtn;
}

// Initialize HD44780 into 4-bit mode

int nk_lcd8574_init(nk_lcd8574_t *dev, bool backlight_on, bool f_bit, bool n_bit)
{
    int rtn = 0;

    if (backlight_on)
        dev->regval = LCD8574_BACKLIGHT | HD44780_FUNC | HD44780_FUNC_DL;
    else
        dev->regval = HD44780_FUNC | HD44780_FUNC_DL;

    // Reset sequence to put LCD into 8-bit mode
    nk_udelay(15000); // 15 ms for display to boot
    rtn |= nk_lcd8574_pulse(dev);
    nk_udelay(4100);
    rtn |= nk_lcd8574_pulse(dev);
    nk_udelay(61);
    rtn |= nk_lcd8574_pulse(dev);

    // Switch to 4-bit mode
    dev->regval = (dev->regval & LCD8574_BACKLIGHT) | HD44780_FUNC;
    rtn |= nk_lcd8574_pulse(dev);

    // Set font / no. lines
    rtn |= nk_lcd8574_send_command(dev, HD44780_FUNC | (f_bit ? HD44780_FUNC_F : 0) | (n_bit ? HD44780_FUNC_N : 0));

    // Display off, cursor off, blink off
    rtn |= nk_lcd8574_send_command(dev, HD44780_DISPLAY);

    // Clear screen, home cursor
    rtn |= nk_lcd8574_send_command(dev, HD44780_CLS);
    nk_udelay(1530);

    // Display on, cursor on, blink on
    rtn |= nk_lcd8574_send_command(dev, HD44780_DISPLAY | HD44780_DISPLAY_D | HD44780_DISPLAY_C | HD44780_DISPLAY_B);
 
    return rtn;
}

// Enable / Display backlight

int nk_lcd8574_backlight(nk_lcd8574_t *dev, bool on)
{
    if (on)
        dev->regval |= LCD8574_BACKLIGHT;
    else
        dev->regval &= ~LCD8574_BACKLIGHT;
    return pcf8574_write(dev->pcf8574, dev->regval);
}

// Send HD44780 command byte

int nk_lcd8574_send_command(nk_lcd8574_t *dev, uint8_t cmd)
{
    int rtn = 0;
    dev->regval = (dev->regval & LCD8574_BACKLIGHT) | (cmd & LCD8574_MASK);
    rtn |= nk_lcd8574_pulse(dev);
    dev->regval = (dev->regval & LCD8574_BACKLIGHT) | ((cmd << 4) & LCD8574_MASK);
    rtn |= nk_lcd8574_pulse(dev);
    return rtn;
}

// Write HD44780 text

int nk_lcd8574_send_data(nk_lcd8574_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    while (len--)
    {
        dev->regval = (dev->regval & LCD8574_BACKLIGHT) | (*data & LCD8574_MASK) | LCD8574_RS;
        rtn |= nk_lcd8574_pulse(dev);
        dev->regval = (dev->regval & LCD8574_BACKLIGHT) | ((*data << 4) & LCD8574_MASK) | LCD8574_RS;
        rtn |= nk_lcd8574_pulse(dev);
        ++data;
    }
    return rtn;
}
