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

#include "nkarch.h"
#include "nkdriver_hd44780.h"

// Set display mode

int nk_hd44780_display_mode(const nk_hd44780_t *dev, bool display_on, bool cursor_on, bool cursor_blink)
{
    return dev->send_command(dev->ptr,
        HD44780_DISPLAY |
        (display_on ? HD44780_DISPLAY_D : 0) |
        (cursor_on ? HD44780_DISPLAY_C : 0) |
        (cursor_blink ? HD44780_DISPLAY_B : 0)
    );
}

// Clear screen
int nk_hd44780_cls(const nk_hd44780_t *dev)
{
    int rtn = dev->send_command(dev->ptr, HD44780_CLS);
    nk_udelay(1530);
    return rtn;
}

// Home cursor
int nk_hd44780_home(const nk_hd44780_t *dev)
{
    return dev->send_command(dev->ptr, HD44780_HOME);
}

// Set cursor position
int nk_hd44780_pos(const nk_hd44780_t *dev, uint8_t position)
{
    return dev->send_command(dev->ptr, HD44780_DDRAM + position);
}

// Write data
int nk_hd44780_write(const nk_hd44780_t *dev, uint8_t *data, int len)
{
    return dev->send_data(dev->ptr, data, len);
}
