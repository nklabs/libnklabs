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
#include "nkdatetime.h"

const char *nk_weekday_name[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char *nk_month_name[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/* Days per month, normal year */
static const uint8_t dpm[]= { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Days per month, leap year */
static const uint8_t dpmleap[]= { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Seconds per day */
#define SPD (60*60*24)

// Compute day of week from date

int nk_calc_day_of_week(int y, int m, int d)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    ++m;
    ++d;
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

// Return current minute of day

int nk_calc_minute_of_day(const nkdatetime_t *datetime)
{
    return datetime->hour * 60 + datetime->min;
}

// Return current minute of week

int nk_calc_minute_of_week(const nkdatetime_t *datetime)
{
    return datetime->weekday * 1440 + (datetime->hour * 60) + datetime->min;
}

// Get date/time as a string: YYYY-MM-DD HH:MM:SS

int nk_date_string(char *buf, size_t len, const nkdatetime_t *datetime)
{
    return nk_snprintf(buf, len, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", datetime->year, datetime->month, datetime->day, datetime->hour, datetime->min, datetime->sec);
}

// Sanity check datetime
// If any problems, make datetime have legal value and return -1

int nk_datetime_sanity(nkdatetime_t *datetime)
{
    int rtn = 0;
    if (datetime->sec > 59)
    {
        datetime->min = 0;
        rtn = -1;
    }
    if (datetime->min > 59)
    {
        datetime->min = 0;
        rtn = -1;
    }
    if (datetime->hour > 23)
    {
        datetime->hour = 0;
        rtn = -1;
    }
    if (datetime->weekday > 6)
    {
        datetime->weekday = 0;
        rtn = -1;
    }
    if (datetime->day > 30)
    {
        datetime->day = 0;
        rtn = -1;
    }
    if (datetime->month > 11)
    {
        datetime->month = 0;
        rtn = -1;
    }
    if (datetime->year < 2000)
    {
        // Some MCU RTCs default to 1970
        // But for external RTC, we probably want minimum year to be 2000
        datetime->year = 2000;
    }
    if (datetime->year > 2199)
    {
        datetime->year = 2000;
        rtn = -1;
    }
    return rtn;
}

void nk_datetime_clear(nkdatetime_t *datetime)
{
    datetime->sec = 0;
    datetime->min = 0;
    datetime->hour = 0;
    datetime->weekday = 6;
    datetime->day = 0;
    datetime->month = 0;
    datetime->year = 2000;
}

/* Return true if year is a leap-year */

/* In the Gregorian calendar, each leap year has 366 days instead of the
   usual 365, by extending February to 29 days rather than the common 28. 
   These extra days occur in years which are multiples of four (with the
   exception of years divisible by 100 but not by 400) */

static int isleapyear(int year)
{
    if(year % 4 || (year % 100 == 0 && year % 400))
        return 0;
    else
        return 1;
}

/* Convert nkdatetine_t to -> Seconds since Jan 1, 1970 */

long long nk_datetime_to_unix(const nkdatetime_t *datetime)
{
    long long clock = 0;
    int x;

    /* Valid year? */

    if (datetime->year < 1970)
        return -1;

    /* Adjust for year */

    for (x = 1970; x != datetime->year; x++)
        if (isleapyear(x))
            clock += SPD * 366;
        else
            clock += SPD * 365;

    /* Valid month? */

    if (datetime->month > 11)
        return -1;

    if (isleapyear(datetime->year)) {
        /* Adjust for month */
        for(x = 0; x != datetime->month; x++)
            clock += SPD * dpmleap[x];
        /* Valid day? */
        if (datetime->day >= dpmleap[datetime->month])
            return -1;
    } else {
        /* Adjust for month */
        for (x = 0; x != datetime->month; x++)
            clock += SPD * dpm[x];
        /* Valid day? */
        if (datetime->day >= dpm[datetime->month])
            return -1;
    }

    /* Adjust for day of month */

    clock += SPD * (datetime->day);

    /* Adjust for time */

    if (datetime->hour > 23)
        return -1;

    if (datetime->min > 59)
        return -1;

    if (datetime->sec > 59)
        return -1;
    
    clock += (datetime->hour * 60 * 60) + (datetime->min * 60) + datetime->sec;

    return clock;
}

int nk_unix_to_datetime(long long unixtime, nkdatetime_t *datetime)
{
    int year = 1970;
    int month;
    int days_since_year;
    int sec_of_day;
    int hour_of_day;
    int sec_of_hour;
    int min_of_hour;
    int sec_of_min;

    if (unixtime < 0)
        return -1;

    days_since_year = unixtime / SPD;
    sec_of_day = unixtime - days_since_year * SPD;
    hour_of_day = sec_of_day / 3600;
    sec_of_hour = sec_of_day - hour_of_day * 3600;
    min_of_hour = sec_of_hour / 60;
    sec_of_min = sec_of_hour - min_of_hour * 60;

    while (days_since_year >= (isleapyear(year) ? 366 : 365)) {
        days_since_year -= (isleapyear(year) ? 366 : 365);
        ++year;
    }

    if (isleapyear(year)) {
        for (month = 0; days_since_year >= dpmleap[month]; ++month)
            days_since_year -= dpmleap[month];
    } else {
        for (month = 0; days_since_year >= dpm[month]; ++month)
            days_since_year -= dpm[month];
    }

    datetime->year = year;
    datetime->month = month;
    datetime->day = days_since_year;

    datetime->weekday = nk_calc_day_of_week(year, month, days_since_year);

    datetime->hour = hour_of_day;
    datetime->min = min_of_hour;
    datetime->sec = sec_of_min;
    return 0;
}
