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
#include "nkdriver_ds1307.h"

int nk_ds1307_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime)
{
    uint8_t buf[9];

    buf[0] = 0; // Starting write address
    buf[1 + DS1307_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10); // Clear CH bit
    buf[1 + DS1307_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + DS1307_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[1 + DS1307_REG_DAY] = datetime->weekday + 1; // Day of week
    buf[1 + DS1307_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + DS1307_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century on some of them
    buf[1 + DS1307_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);
    buf[1 + DS1307_REG_CTRL] = DS1307_REG_OUT_BIT; // No SQWE, leave output at hi-z for lowest possible power

    return nk_i2c_write(dev, sizeof(buf), buf);
}

int nk_ds1307_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf[7];
    int rtn;

    nk_datetime_clear(datetime);
    
    buf[0] = 0; // Set starting address
    rtn = nk_i2c_write(dev, 1, buf);

    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read date/time and status
    rtn = nk_i2c_read(dev, sizeof(buf), buf);

    if (rtn)
        return rtn;

    datetime->sec = (0x0F & buf[DS1307_REG_SECONDS]) + ((0x7 & (buf[DS1307_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[DS1307_REG_MINUTES]) + ((0x07 & (buf[DS1307_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[DS1307_REG_HOURS]) + ((0x3 & (buf[DS1307_REG_HOURS] >> 4)) * 10);
    datetime->weekday = (0x07 & buf[DS1307_REG_DAY]) - 1;
    datetime->day = (0x0F & buf[DS1307_REG_DATE]) + ((0x03 & (buf[DS1307_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[DS1307_REG_MONTH]) + ((0x1 & (buf[DS1307_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[DS1307_REG_YEAR]) + ((buf[DS1307_REG_YEAR] >> 4) * 10) + 2000;

    // Sanity check the date- so we don't crash
    rtn = nk_datetime_sanity(datetime);
    if (rtn)
    {
        return NK_ERROR_TIME_LOST;
    }

    if (buf[DS1307_REG_SECONDS] & DS1307_REG_CH_BIT)
    {
        return NK_ERROR_TIME_LOST;
    }

    return 0;
}
