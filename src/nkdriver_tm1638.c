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

// TM1638 7-segment display driver

// This is not I2C: bits are backwards and there is no address byte

#include <string.h>
#include "nkfont_7seg.h"
#include "nkdriver_tm1638.h"

#define SEG_EXTRA1 (0x100)
#define SEG_EXTRA2 (0x200)

// Some displays have large capacitors on the SDA / SCK lines,
// so rising edge is quite slow.

int nk_tm1638_init(const nk_tm1638_t *dev)
{
    nk_pin_write(dev->clk, 1);
    nk_pin_write(dev->dio, 1);
    nk_pin_write(dev->stb, 1);
    nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_pin_setmode(dev->stb, NK_PINMODE_INPUT_PULLUP);
    return 0;
}

int nk_tm1638_start(const nk_tm1638_t *dev)
{
    nk_pin_setmode(dev->stb, NK_PINMODE_OUTPUT);
    nk_pin_write(dev->stb, 0);
    nk_udelay(dev->tfall + 1);
    return 0;
}

int nk_tm1638_stop(const nk_tm1638_t *dev)
{
    nk_pin_setmode(dev->stb, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);
    return 0;
}

int nk_tm1638_write_byte(const nk_tm1638_t *dev, uint8_t byte)
{
    int x;
    int prev = 0;
    for (x = 0; x != 8; ++x)
    {
        if (byte & 1)
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
            if (prev)
                nk_udelay(1); // Previous bit was high, so save time
            else
                nk_udelay(dev->trise + 1);
            prev = 1;
        }
        else
        {
            nk_pin_setmode(dev->dio, NK_PINMODE_OUTPUT);
            nk_pin_write(dev->dio, 0);
            if (prev)
                nk_udelay(dev->tfall + 1);
            else
                nk_udelay(1); // Previous bit was low, so save time
            prev = 0;
        }
        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1);
        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
        byte >>= 1;
    }

    return 0;
}

int nk_tm1638_read_byte(const nk_tm1638_t *dev, uint8_t *byte)
{
    bool pinv;
    int x;
    uint8_t val = 0;

    nk_pin_setmode(dev->dio, NK_PINMODE_INPUT_PULLUP);
    nk_udelay(dev->trise + 1);

    for (x = 0; x != 8; ++x)
    {
        val >>= 1;

        nk_pin_setmode(dev->clk, NK_PINMODE_OUTPUT);
        nk_pin_write(dev->clk, 0);
        nk_udelay(dev->tfall + 1); // Delay for clock to fall

        nk_udelay(dev->trise + 1); // Delay for data to rise

        nk_pin_read(dev->dio, &pinv); // Check input
        if (pinv)
            val |= 0x80;

        nk_pin_setmode(dev->clk, NK_PINMODE_INPUT_PULLUP);
        nk_udelay(dev->trise + 1);
    }

    *byte = val;
    return 0;
}

int nk_tm1638_write(const nk_tm1638_t *dev, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1638_start(dev);
    while (len)
    {
        rtn |= nk_tm1638_write_byte(dev, *data);
        ++data;
        --len;
    }
    nk_tm1638_stop(dev);
    return rtn;
}

int nk_tm1638_read(const nk_tm1638_t *dev, uint8_t cmd, uint8_t *data, int len)
{
    int rtn = 0;
    nk_tm1638_start(dev);
    rtn |= nk_tm1638_write_byte(dev, cmd);

    while (len)
    {
        rtn |= nk_tm1638_read_byte(dev, data);
        ++data;
        --len;
    }
    nk_tm1638_stop(dev);
    return rtn;
}

int nk_tm1638_display_raw(const nk_tm1638_t *dev, const uint16_t *digits)
{
    int x, y;
    uint8_t data[17];
    int rtn;
    data[0] = TM1638_MODE_LOAD;
    rtn = nk_tm1638_write(dev, data, 1);

    if (!dev->ca)
    {
        // 8 common cathode digits
        // Up to 8 digits could be supported, 10 segments per digit
        for (x = 0; x != 8; ++x)
        {
            data[1 + x*2] = digits[x];
            data[1 + x*2 + 1] = (digits[x] >> 8);
        }
    }
    else
    {
        // 10 common anode digits
        memset(data , 0, sizeof(data));
        for (y = 0; y != 10; ++y) // Input digit
        {
            for (x = 0; x != 8; ++x) // Input segment
            {
                if (digits[y] & (1 << x)) // Is segment x or digit y set?
                {
                    uint16_t dig = (1 << y); // Set it in the output
                    data[1 + 2*x] |= dig;
                    data[1 + 2*x + 1] |= (dig >> 8);
                }
            }
        }
    }

    data[0] = TM1638_ADDR;

    rtn |= nk_tm1638_write(dev, data, 17);
    return rtn;
}

int nk_tm1638_display(const nk_tm1638_t *dev, uint8_t dp, uint8_t extra1, uint8_t extra2, const uint8_t *digits)
{
    uint16_t data[8];
    int x;
    int no_digits = (dev->ca ? 10 : 8);
    for (x = 0; x != no_digits; ++x)
    {
        uint16_t val = nkfont_7seg[digits[x] & 0x7F];
        // Decimal point
        if (dp & (1 << x))
            val |= SEG_DP;
        // Extra segments
        if (extra1 & (1 << x))
            val |= SEG_EXTRA1;
        if (extra2 & (1 << x))
            val |= SEG_EXTRA2;
        data[dev->digit_map[x]] = val;
    }
    return nk_tm1638_display_raw(dev, data);
}

int nk_tm1638_display_off(const nk_tm1638_t *dev)
{
    uint8_t data[1];
    data[0] = TM1638_DISP_OFF;
    return nk_tm1638_write(dev, data, 1);
}

int nk_tm1638_display_on(const nk_tm1638_t *dev, uint8_t brightness)
{
    uint8_t data[1];
    data[0] = TM1638_DISP_ON + brightness;
    return nk_tm1638_write(dev, data, 1);
}

int nk_tm1638_keyscan(const nk_tm1638_t *dev, uint32_t *data)
{
    uint8_t val[4];
    int rtn = nk_tm1638_read(dev, TM1638_MODE_READ, val, 4);
    *data = val[0] + (val[1] << 8) + (val[2] << 16) + (val[3] << 24);
    return rtn;
}
