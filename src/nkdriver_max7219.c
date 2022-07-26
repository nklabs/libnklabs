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

#include <string.h>
#include "nkfont_7seg.h"
#include "nkdriver_max7219.h"

int nk_max7219_write(const nk_max7219_t *dev, uint16_t data)
{
    int x;
    int prev = -1;

    nk_pin_write(dev->load, 0);
    nk_udelay(dev->tfall + 1);

    for (x = 0; x != 16; ++x)
    {
        if (data & 0x8000)
        {
            nk_pin_write(dev->mosi, 1);
            if (!prev)
                nk_udelay(dev->trise );
            prev = 1;
        }
        else
        {
            nk_pin_write(dev->mosi, 0);
            if (prev)
                nk_udelay(dev->tfall);
            prev = 0;
        }

        nk_pin_write(dev->sclk, 0);
        nk_udelay(dev->tfall + 1);

        nk_pin_write(dev->sclk, 1);
        nk_udelay(dev->trise + 1);

        data <<= 1;
    }

    nk_pin_write(dev->load, 1);
    nk_udelay(dev->trise + 1);
    return 0;
}

int nk_max7219_init(const nk_max7219_t *dev)
{
    int rtn;
    nk_pin_setmode(dev->load, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->load, 1);

    nk_pin_setmode(dev->sclk, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->sclk, 1);

    nk_pin_setmode(dev->mosi, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->mosi, 1);

    rtn = nk_max7219_write(dev, MAX7219_CMD_NO_TEST_MODE);
    rtn |= nk_max7219_write(dev, MAX7219_CMD_DECODE_MODE_NONE);
    rtn |= nk_max7219_write(dev, MAX7219_CMD_INTENSITY + 8);
    rtn |= nk_max7219_write(dev, MAX7219_CMD_SCAN_LIMIT_ALL);
    rtn |= nk_max7219_write(dev, MAX7219_CMD_NO_SHUTDOWN);
    
    return rtn;
}

int nk_max7219_display_raw(const nk_max7219_t *dev, const uint8_t *digits)
{
    int rtn = 0;
    int x;
    for (x = 0; x != 8; ++x)
    {
        rtn |= nk_max7219_write(dev, MAX7219_CMD_DIGIT0 + (x << 8) + digits[x]);
    }
    return rtn;
}

int nk_max7219_display(const nk_max7219_t *dev, uint8_t dp, const uint8_t *digits)
{
    uint8_t data[8];
    int x;
    for (x = 0; x != 8; ++x)
    {
        uint8_t v = nkfont_7seg[digits[x] & 0x7F];
        uint8_t val = 0;
        // Decimal point
        if (dp & (1 << x))
            val |= SEG_DP;

        // Segments are reversed for MAX7219: D6 has A instead of D0
        int z;
        for (z = 0; z != 7; ++z)
            if (v & (1 << z))
                val |= (0x40 >> z);

        data[dev->digit_map[x]] = val;
    }
    return nk_max7219_display_raw(dev, data);
}

int nk_max7219_display_on(const nk_max7219_t *dev, uint8_t brightness)
{
    int rtn = nk_max7219_write(dev, MAX7219_CMD_INTENSITY + brightness);
    rtn |= nk_max7219_write(dev, MAX7219_CMD_NO_SHUTDOWN);
    return rtn;
}

int nk_max7219_display_off(const nk_max7219_t *dev)
{
    return nk_max7219_write(dev, MAX7219_CMD_SHUTDOWN);
}
