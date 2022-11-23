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

// Interface to SPI RTC DS1391U-33+

#include <string.h>
#include "nkrtc.h"
#include "nkdriver_ds1391.h"

int nk_ds3232_rtc_set_datetime(const nkspi_device_t *dev, nkdatetime_t *datetime)
{
    uint8_t buf[9];
    int rtn;

    buf[0] = 0x80; // Write to RTC array
    buf[1] = 0;
    buf[2] = ((datetime->sec / 10) << 4) + (datetime->sec % 10);
    buf[3] = ((datetime->min / 10) << 4) + (datetime->min % 10);
    buf[4] = ((datetime->hour / 10) << 4) + (datetime->hour % 10); // 24 hour mode: bit 6 is 0
    buf[5] = datetime->weekday + 1; // Day of week
    buf[6] = (((datetime->day + 1) / 10) << 4) + ((datetime->day + 1) % 10);
    buf[7] = (((datetime->month + 1) / 10) << 4) + ((datetime->month + 1) % 10); // Bit 7 is century
    buf[8] = (((datetime->year - 2000) / 10) << 4) + ((datetime->year - 2000) % 10);

    rtn = nk_spi_transfer(dev, buf, sizeof(buf));

    return rtn;
}

int nk_ds1391_get_datetime(const nkspi_device_t *dev, nkdatetime_t *datetime)
{
    int rtn;
    uint8_t buf[17];

    nk_datetime_clear(datetime);

    memset(buf, 0, sizeof(buf));
    buf[0] = 0x00; // Read from RTC array

    rtn = nk_spi_transfer(dev, buf, sizeof(buf));
    if (rtn)
        return rtn;

    if (buf[0x0F] & 0x80) {
        // nk_printf("OSF bit was set! Clearing RTC...\n");
        buf[0] = 0x80; // Write to RTC array
        buf[1] = 0; // Tenths
        buf[2] = 0; // Second
        buf[3] = 0; // Minutes
        buf[4] = 0; // Hours
        buf[5] = 0x07; // Day of week
        buf[6] = 0x01; // Day of month
        buf[7] = 0x01; // Month
        buf[8] = 0x00; // Year
        buf[9] = 0x0; // Alarm tenths
        buf[10] = 0x0; // Alarm seconds
        buf[11] = 0x0; // Alarm minutes
        buf[12] = 0x0; // Alarm hours
        buf[13] = 0x01; // Alarm day of month
        buf[14] = 0x00; // Enable oscillator
        buf[15] = 0x00; // Clear OSF
        buf[16] = 0x00; // Disable trickle charger
        rtn = nk_spi_transfer(dev, buf, sizeof(buf));
        return rtn;
    }

    datetime->sec = (0x0F & buf[2]) + ((0x7 & (buf[2] >> 4)) * 10);
    datetime->min = (0x0F & buf[3]) + ((0x07 & (buf[3] >> 4)) * 10);
    datetime->hour = (0x0F & buf[4]) + ((0x3 & (buf[4] >> 4)) * 10);
    datetime->weekday = (0x0F & buf[5]) - 1;
    datetime->day = (0x0F & buf[6]) + ((0x03 & (buf[6] >> 4)) * 10) - 1;
    datetime->month = (0x0F & buf[7]) + ((0x1 & (buf[7] >> 4)) * 10) - 1;
    datetime->year = (0x0F & buf[8]) + ((buf[8] >> 4) * 10) + 2000;

    return rtn;
}
