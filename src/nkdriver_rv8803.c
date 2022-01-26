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

#define RV8803_VALID_CODE 0x5A

int nk_rv8803_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime)
{
    uint8_t buf[9];

    buf[0] = 0; // Starting write address
    buf[1 + RV8803_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[1 + RV8803_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + RV8803_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[1 + RV8803_REG_DAY] = (1 << datetime->weekday); // Day of week
    buf[1 + RV8803_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + RV8803_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century on some of them
    buf[1 + RV8803_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);
    buf[1 + RV8803_REG_RAM] = RV8803_VALID_CODE;

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

int nk_rv8803_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf_2[8];
    uint8_t buf[8];
    int rtn;

    nk_datetime_clear(datetime);
    
    buf_2[0] = 0; // Set starting address
    rtn = nk_i2c_write(dev, 1, buf_2);

    if (rtn)
        return rtn;

    memset(buf_2, 0, sizeof(buf_2));

    // Read date/time and status
    rtn = nk_i2c_read(dev, 8, buf_2);

    if (rtn)
        return rtn;


    buf[0] = 0; // Set starting address
    rtn = nk_i2c_write(dev, 1, buf);

    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read date/time and status a second time
    rtn = nk_i2c_read(dev, 8, buf);

    if (rtn)
        return rtn;


    if (buf[RV8803_REG_SECONDS] != buf_2[RV8803_REG_SECONDS])
    {
        // Not in the same second? Read one more time...
        buf[0] = 0; // Set starting address
        rtn = nk_i2c_write(dev, 1, buf);

        if (rtn)
            return rtn;

        memset(buf, 0, sizeof(buf));

        // Read date/time and status a third time
        rtn = nk_i2c_read(dev, 8, buf);

        if (rtn)
            return rtn;
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

    if (buf[RV8803_REG_RAM] != RV8803_VALID_CODE)
    {
        return NK_ERROR_TIME_LOST;
    }

    return 0;
}
