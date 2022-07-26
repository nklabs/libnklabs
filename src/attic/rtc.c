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
#include "nkcli.h"

int ext_rtc_set_time(int year, int month, int day, int hour, int min, int sec)
{
    uint8_t buf[9];


    buf[0] = 0x80; // Write to RTC array
    buf[1] = 0;
    buf[2] = ((sec / 10) << 4) + (sec % 10);
    buf[3] = ((min / 10) << 4) + (min % 10);
    buf[4] = ((hour / 10) << 4) + (hour % 10); // 24 hour mode: bit 6 is 0
    buf[5] = 0x01; // Day of week
    buf[6] = ((day / 10) << 4) + (day % 10);
    buf[7] = ((month / 10) << 4) + (month % 10); // Bit 7 is century
    buf[8] = (((year - 2000) / 10) << 4) + ((year - 2000) % 10);

    if (rtc_spi_transfer(buf, sizeof(buf)))
        return -1;

    return 0;
}

void ext_rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    uint8_t buf[17];

    memset(buf, 0, sizeof(buf));
    buf[0] = 0x00; // Read from RTC array

    if (rtc_spi_transfer(buf, sizeof(buf))) {
        nk_printf("RTC SPI transfer error\n");
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;
        return;
    }

    if (buf[0x0F] & 0x80) {
        nk_printf("OSF bit was set! Clearing RTC...\n");
        buf[0] = 0x80; // Write to RTC array
        buf[1] = 0; // Tenths
        buf[2] = 0; // Second
        buf[3] = 0; // Minutes
        buf[4] = 0; // Hours
        buf[5] = 0x01; // Day of week
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
        if (rtc_spi_transfer(buf, sizeof(buf))) {
            nk_printf("RTC SPI transfer error\n");
        }
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;
        return;
    }

    *sec = (0x0F & buf[2]) + ((0x7 & (buf[2] >> 4)) * 10);
    *min = (0x0F & buf[3]) + ((0x07 & (buf[3] >> 4)) * 10);
    *hour = (0x0F & buf[4]) + ((0x3 & (buf[4] >> 4)) * 10);
    *day = (0x0F & buf[6]) + ((0x03 & (buf[6] >> 4)) * 10);
    *month = (0x0F & buf[7]) + ((0x1 & (buf[7] >> 4)) * 10);
    *year = (0x0F & buf[8]) + ((buf[8] >> 4) * 10) + 2000;

    return;
}

/* Set internal clock from external clock */

int set_from_hwclock()
{
    int year, month, day, hour, min, sec;
    int now;
    nk_startup_message("Set time from external RTC\n");
    ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
    now = datetime_to_unix(year, month, day, hour, min, sec);
    if (now != -1) {
        Seconds_set(now);
        nk_printf("RTC DateTime is %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
        return 0;
    } else {
        nk_printf("RTC has invalid date/time, time not set\n");
        return -1;
    }
}

/* Set external clock from internal clock */

int set_to_hwclock()
{
    int year, month, day, hour, min, sec;
    int now;
    now = Seconds_get();
    if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
        nk_printf("Error converting UNIX Epoch time to UTC\n");
    } else {
        if (ext_rtc_set_time(year, month, day, hour, min, sec)) {
            nk_printf("Error writing to battery backed real time clock\n");
        } else {
            nk_printf("Battery backed real time clock has been set\n");
            return 0;
        }
    }
    return -1;
}

void rtc_init()
{
    nk_startup_message("Real time clock\n");
    set_from_hwclock();
}

int cmd_hwclock(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (facmode && nk_fscan(args, "-w %e")) {
        set_to_hwclock();
    } else if (facmode && nk_fscan(args, "")) {
        ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
        nk_printf("External clock reports %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", year, month, day, hour, min, sec);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

int cmd_date(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    int now;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d ", &year, &month, &day, &hour, &min, &sec)) {
        now = datetime_to_unix(year, month, day, hour, min, sec);
        if (now < 0) {
            nk_printf("Error converting UTC to UNIX Epoc time\n");
        } else {
            Seconds_set(now);
            nk_printf("Time set\n");
            set_to_hwclock();
        }
    } else if (nk_fscan(args, "")) {
        now = Seconds_get();
        if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
            nk_printf("Error converting UNIX Epoch time to UTC\n");
        }
        nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

void date_string(char *buf)
{
    int year, month, day, hour, min, sec;
    int now;
    now = Seconds_get();
    if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
        nk_printf("Error converting UNIX Epoch time to UTC\n");
    }
    nk_snprintf(buf, 30, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ", year, month, day, hour, min, sec);
}
