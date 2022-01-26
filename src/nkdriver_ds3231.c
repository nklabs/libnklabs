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
#include "nkdriver_ds3231.h"

int nk_ds3231_set_datetime(const nk_i2c_device_t *dev, const nkdatetime_t *datetime)
{
    uint8_t buf[17];

    buf[0] = 0; // Starting write address
    buf[1 + DS3231_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[1 + DS3231_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + DS3231_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[1 + DS3231_REG_DAY] = datetime->weekday + 1; // Day of week
    buf[1 + DS3231_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + DS3231_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century on some of them
    buf[1 + DS3231_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);

    buf[1 + DS3231_REG_AL1_SECONDS] = 0;
    buf[1 + DS3231_REG_AL1_MINUTES] = 0;
    buf[1 + DS3231_REG_AL1_HOUR] = 0;
    buf[1 + DS3231_REG_AL1_DAY] = 0;
    buf[1 + DS3231_REG_AL2_MINUTES] = 0;
    buf[1 + DS3231_REG_AL2_HOUR] = 0;
    buf[1 + DS3231_REG_AL2_DAY] = 0;
    buf[1 + DS3231_REG_CONTROL] = DS3231_REG_CONTROL_INTCN_BIT; // Route interrupt to out pin, disable square wave for min. power
    buf[1 + DS3231_REG_STATUS] = 0; // Clear OSF bit

    return nk_i2c_write(dev, sizeof(buf), buf);
}

int nk_ds3231_get_temp(const nk_i2c_device_t *dev, float *temp)
{
    int16_t t;
    uint8_t buf[2];
    int rtn;
    buf[0] = 0x11; // Set starting address

    rtn = nk_i2c_write(dev, 1, buf);
    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read date/time and status
    rtn = nk_i2c_read(dev, sizeof(buf), buf);

    if (rtn)
        return rtn;

    t = (buf[0] << 8) + buf[1];

    *temp = (float)t * .00390625f; // .25 C / 64

    return 0;
}

int nk_ds3231_get_datetime(const nk_i2c_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf[16];
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

    datetime->sec = (0x0F & buf[DS3231_REG_SECONDS]) + ((0x7 & (buf[DS3231_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[DS3231_REG_MINUTES]) + ((0x07 & (buf[DS3231_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[DS3231_REG_HOURS]) + ((0x3 & (buf[DS3231_REG_HOURS] >> 4)) * 10);
    datetime->weekday = (0x07 & buf[DS3231_REG_DAY]) - 1;
    datetime->day = (0x0F & buf[DS3231_REG_DATE]) + ((0x03 & (buf[DS3231_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[DS3231_REG_MONTH]) + ((0x1 & (buf[DS3231_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[DS3231_REG_YEAR]) + ((buf[DS3231_REG_YEAR] >> 4) * 10) + 2000;

    // Sanity check the date- so we don't crash
    rtn = nk_datetime_sanity(datetime);
    if (rtn)
    {
        return NK_ERROR_TIME_LOST;
    }

    if (buf[DS3231_REG_STATUS] & DS3231_REG_STATUS_OSF_BIT)
    {
        return NK_ERROR_TIME_LOST;
    }

    return 0;
}
