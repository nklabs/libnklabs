// Copyright 2020 NK Labs, LLC

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
#include "nkrtc.h"
#include "nkdriver_rv8803.h"

int nk_rv8803_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime)
{
    uint8_t buf[17];

    buf[0] = 0; // Starting write address
    buf[1 + RV8803_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[1 + RV8803_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + RV8803_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[1 + RV8803_REG_DAY] = (1 << datetime->weekday); // Day of week
    buf[1 + RV8803_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + RV8803_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century on some of them
    buf[1 + RV8803_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);

    buf[1 + RV8803_REG_RAM] = 0;
    buf[1 + RV8803_REG_AL1_MINUTES] = 0;
    buf[1 + RV8803_REG_AL1_HOURS] = 0;
    buf[1 + RV8803_REG_AL1_DAY] = 0;
    buf[1 + RV8803_REG_TIMER0] = 0;
    buf[1 + RV8803_REG_TIMER1] = 0;
    buf[1 + RV8803_REG_EXTENSION] = 0;
    buf[1 + RV8803_REG_FLAGS] = 0; // Clear low voltage flags
    buf[1 + RV8803_REG_CONTROL] = 0;

    return nk_i2c_write(dev, sizeof(buf), buf);
}

uint8_t onehot_to_bin(uint8_t v)
{
    uint8_t x;
    for (x = 0; x != 8; ++x)
        if (v & 1)
            return x;
        else
            v >>= 1;
    return 0;
}

static int nk_rv8803_read(nk_i2c_device_t *dev, uint8_t *buf, int len)
{
    int rtn = 0;
    int retry;

    memset(buf, 0, len);

    // Some versions of rv8803 have a read bug, so we need to retry here
    for (retry = 0; retry != 2; ++retry)
    {
        uint8_t addr = 0;
        rtn = nk_i2c_write(dev, 1, &addr);

        if (!rtn)
        {
            rtn = nk_i2c_read(dev, len, buf);
            if (!rtn)
                break;
        }
    }

    return rtn;
}

int nk_rv8803_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf[15];
    uint8_t buf_2[15];
    int rtn;

    nk_datetime_clear(datetime);

    rtn = nk_rv8803_read(dev, buf, sizeof(buf));
    
    if (rtn)
        return rtn;

    if ((0x7F & buf[RV8803_REG_SECONDS]) == 0x59)
    {
        rtn = nk_rv8803_read(dev, buf_2, sizeof(buf_2));

        if (rtn)
            return rtn;

        if ((0x7F & buf_2[RV8803_REG_SECONDS]) == 0x59)
        {
            // Wrap did not happen, so first reading was good
        }
        else
        {
            // Yup, we wrapped.  Use second reading
            memcpy(buf, buf_2, sizeof(buf));
        }
    }

    datetime->sec = (0x0F & buf[RV8803_REG_SECONDS]) + ((0x7 & (buf[RV8803_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[RV8803_REG_MINUTES]) + ((0x07 & (buf[RV8803_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[RV8803_REG_HOURS]) + ((0x3 & (buf[RV8803_REG_HOURS] >> 4)) * 10);
    datetime->weekday = onehot_to_bin(buf[RV8803_REG_DAY]);
    datetime->day = (0x0F & buf[RV8803_REG_DATE]) + ((0x03 & (buf[RV8803_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[RV8803_REG_MONTH]) + ((0x1 & (buf[RV8803_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[RV8803_REG_YEAR]) + ((buf[RV8803_REG_YEAR] >> 4) * 10) + 2000;

    // Sanity check the date- so we don't crash
    rtn = nk_datetime_sanity(datetime);
    if (rtn)
    {
        return NK_ERROR_TIME_LOST;
    }

    // Invalidate if low voltage flags are set
    if (buf[RV8803_REG_FLAGS] & (RV8803_V2F_BIT | RV8803_V1F_BIT))
    {
        return NK_ERROR_TIME_LOST;
    }

    return 0;
}
