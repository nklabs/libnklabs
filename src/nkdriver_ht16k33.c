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

#include "nkdriver_ht16k33.h"
#include "nkfont_14seg.h"

int nk_ht16k33_init(const nk_i2c_device_t *dev)
{
    int rtn;
    uint8_t buf[1];

    // Enable oscillator
    buf[0] = HT16K33_CMD_SETUP | HT16K33_S_BIT;
    rtn = nk_i2c_write(dev, 1, buf);

    // Enable display
    buf[0] = HT16K33_CMD_DISP_SETUP | HT16K33_D_BIT | HT16K33_BLINK_OFF;
    rtn |= nk_i2c_write(dev, 1, buf);

    // Set brightness    
    buf[0] = HT16K33_CMD_DIMMING | 8;
    rtn |= nk_i2c_write(dev, 1, buf);

    return rtn;
}

int nk_ht16k33_display_raw(const nk_i2c_device_t *dev, const uint16_t *digits)
{
    uint8_t buf[17];
    int x;
    buf[0] = HT16K33_CMD_DATA;
    for (x = 0; x != 8; ++x)
    {
        buf[1 + 2*x + 0] = digits[x];
        buf[1 + 2*x + 1] = (digits[x] >> 8);
    }
    return nk_i2c_write(dev, 17, buf);
}

int nk_ht16k33_display(const nk_i2c_device_t *dev, uint8_t dp, const uint8_t *digits)
{
    uint16_t data[8];
    int x;
    for (x = 0; x != 8; ++x)
    {
        uint16_t val = nkfont_14seg[digits[x] & 0x7F];
        // Decimal point
        if (dp & (1 << x))
            val |= SEG14_DP;
        data[x] = val;
    }
    return nk_ht16k33_display_raw(dev, data);
}

int nk_ht16k33_display_on(const nk_i2c_device_t *dev, uint8_t brightness)
{
    int rtn;
    uint8_t buf[1];

    // Display on
    buf[0] = HT16K33_CMD_DISP_SETUP | HT16K33_D_BIT | HT16K33_BLINK_OFF;
    rtn = nk_i2c_write(dev, 1, buf);

    // Set brightness    
    buf[0] = HT16K33_CMD_DIMMING | (brightness & HT16K33_DIM_MASK);
    rtn |= nk_i2c_write(dev, 1, buf);

    return rtn;
}

int nk_ht16k33_display_off(const nk_i2c_device_t *dev)
{
    uint8_t buf[1];
    buf[0] = HT16K33_CMD_DISP_SETUP | HT16K33_BLINK_OFF;
    return nk_i2c_write(dev, 1, buf);
}

int nk_ht16k33_keyscan(const nk_i2c_device_t *dev, uint8_t *data)
{
    int rtn;
    uint8_t buf[1];
    buf[0] = HT16K33_CMD_KEY;
    rtn = nk_i2c_write(dev, 1, buf);
    rtn |= nk_i2c_read(dev, 6, data);
    return rtn;
}
