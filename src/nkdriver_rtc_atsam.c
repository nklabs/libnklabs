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

#include "nkprintf.h"
#include "nkdriver_rtc_atsam.h"

// ATSAM MCU RTC

int nk_mcu_rtc_set_datetime(struct calendar_descriptor *port, const nkdatetime_t *datetime)
{
    // ATSAM
    struct calendar_date_time tim;
    int rtn;

    tim.time.sec = datetime->sec;
    tim.time.min = datetime->min;
    tim.time.hour = datetime->hour;
    tim.date.day = datetime->day + 1;
    tim.date.month = datetime->month + 1;
    tim.date.year = datetime->year;

    rtn = calendar_set_date(&CALENDAR_0, &tim.date);
    rtn = calendar_set_time(&CALENDAR_0, &tim.time);

    return rtn;
}

int nk_mcu_rtc_get_datetime(struct calendar_descriptor *port, nkdatetime_t *datetime)
{
    // ATSAM
    struct calendar_date_time tim;
    int rtn;
    int rtn1;

    rtn = calendar_get_date_time(port, &tim);

    datetime->sec = tim.time.sec;
    datetime->min = tim.time.min;
    datetime->hour = tim.time.hour;
    datetime->weekday = 0;
    datetime->day = tim.date.day - 1;
    datetime->month = tim.date.month - 1;
    datetime->year = tim.date.year;

    rtn1 = nk_datetime_sanity(datetime);

    // Return a valid datetime even if we fail
    return rtn ? rtn : rtn1;
}

int nk_mcu_rtc_init(struct calendar_descriptor *port)
{
    nk_startup_message("MCU Real Time Clock\n");
    calendar_enable(port);
    return 0;
}
