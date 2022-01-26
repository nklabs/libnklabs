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
#include "nkdriver_pcf8523.h"

// PCF8523 Real Time Clock

int nk_pcf8523_set_datetime(nk_i2c_device_t *dev, const nkdatetime_t *datetime)
{
    uint8_t buf[21];

    memset(buf, 0, sizeof(buf));

    buf[0] = 0; // Starting write address
    buf[1 + PCF8523_REG_CONTROL_1] = 0x00; // 7 pF
    buf[1 + PCF8523_REG_CONTROL_2] = 0x00;
    buf[1 + PCF8523_REG_CONTROL_3] = 0x00;
    buf[1 + PCF8523_REG_SECONDS] = ((datetime->sec / 10) << 4) + (datetime->sec % 10); // Clear OS bit
    buf[1 + PCF8523_REG_MINUTES] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[1 + PCF8523_REG_HOURS] = ((datetime->hour / 10) << 4) + (datetime->hour % 10);
    buf[1 + PCF8523_REG_DATE] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[1 + PCF8523_REG_WEEKDAY] = datetime->weekday ; // Day of week
    buf[1 + PCF8523_REG_MONTH] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10);
    buf[1 + PCF8523_REG_YEAR] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);
    buf[1 + PCF8523_REG_AL_MINUTES] = 0x00;
    buf[1 + PCF8523_REG_AL_HOURS] = 0x00;
    buf[1 + PCF8523_REG_AL_DATE] = 0x00;
    buf[1 + PCF8523_REG_AL_WEEKDAY] = 0x00;
    buf[1 + PCF8523_REG_OFFSET] = 0x00;
    buf[1 + PCF8523_REG_CLKOUT] = (7 << PCF8523_COF_SHIFT); // CLKOUT is Hi-Z
    buf[1 + PCF8523_REG_TMRA_FREQ] = 0x00;
    buf[1 + PCF8523_REG_TMRA] = 0x00;
    buf[1 + PCF8523_REG_TMRB_FREQ] = 0x00;
    buf[1 + PCF8523_REG_TMRB] = 0x00;

    return nk_i2c_write(dev, sizeof(buf), buf);
}

int nk_pcf8523_get_datetime(nk_i2c_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf[10];
    int rtn = 0;

    nk_datetime_clear(datetime);

    buf[0] = 0; // Set starting address
    rtn = nk_i2c_write(dev, 1, buf);

    if (rtn)
        return rtn;

    memset(buf, 0, sizeof(buf));

    // Read everything
    rtn = nk_i2c_read(dev, sizeof(buf), buf);

    if (rtn)
        return rtn;

    datetime->sec = (0x0F & buf[PCF8523_REG_SECONDS]) + ((0x7 & (buf[PCF8523_REG_SECONDS] >> 4)) * 10);
    datetime->min = (0x0F & buf[PCF8523_REG_MINUTES]) + ((0x07 & (buf[PCF8523_REG_MINUTES] >> 4)) * 10);
    datetime->hour = (0x0F & buf[PCF8523_REG_HOURS]) + ((0x3 & (buf[PCF8523_REG_HOURS] >> 4)) * 10);
    datetime->weekday = (0x07 & buf[PCF8523_REG_WEEKDAY]);
    datetime->day = (0x0F & buf[PCF8523_REG_DATE]) + ((0x03 & (buf[PCF8523_REG_DATE] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[PCF8523_REG_MONTH]) + ((0x1 & (buf[PCF8523_REG_MONTH] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[PCF8523_REG_YEAR]) + ((buf[PCF8523_REG_YEAR] >> 4) * 10) + 2000;

    rtn = nk_datetime_sanity(datetime);
    if (rtn)
    {
        return NK_ERROR_TIME_LOST;
    }

    if (buf[PCF8523_REG_SECONDS] & PCF8523_OS_BIT)
    {
        return NK_ERROR_TIME_LOST;
    }

    return 0;
}
