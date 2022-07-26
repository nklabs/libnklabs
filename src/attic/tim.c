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


/* Convert between UNIX Epoch time and UTC */

#include "lib.h"

// #define DEBUGTIM 1

/* Time conversion */

/* Days per month, normal year */
static int dpm[]= { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Days per month, leap year */
static int dpmleap[]= { 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/* Seconds per day */
#define SPD (60*60*24)

/* Return true if year is a leap-year */

/* In the Gregorian calendar, each leap year has 366 days instead of the
   usual 365, by extending February to 29 days rather than the common 28. 
   These extra days occur in years which are multiples of four (with the
   exception of years divisible by 100 but not by 400) */

static int isleapyear(int year)
{
    if(year % 4 || year % 100 == 0 && year % 400)
        return 0;
    else
        return 1;
}

/* Day Month Year -> Seconds */

int datetime_to_unix(int year, int month, int day, int hour, int min, int sec)
{
    int clock = 0;
    int x;
    int isleap = isleapyear(year);

    /* Valid year? */

    if (year < 1970 || year > 2037)
        return -1;

    /* Adjust for year */

    for (x = 1970; x != year; x++)
        if (isleapyear(x))
            clock += SPD * 366;
        else
            clock += SPD * 365;

    /* Valid month? */

    if (month < 1 || month > 12)
        return -1;

    if (isleapyear(year)) {
        /* Adjust for month */
        for(x = 1; x != month; x++)
            clock += SPD * dpmleap[x];
        /* Valid day? */
        if (day < 1 || day > dpmleap[month])
            return -1;
    } else {
        /* Adjust for month */
        for (x = 1; x != month; x++)
            clock += SPD * dpm[x];
        /* Valid day? */
        if (day < 1 || day > dpm[month])
            return -1;
    }

    /* Adjust for day of month */

    clock += SPD * (day - 1);

    /* Adjust for time */

    if (hour < 0 || hour > 23)
        return -1;

    if (min < 0 || min > 59)
        return -1;

    if (sec < 0 || sec > 59)
        return -1;
    
    clock += (hour * 60 * 60) + (min * 60) + sec;

    return clock;
}

/* YYYYMMDDHHMMSS -> Seconds since Jan 1, 1970 */

int sdatetime_to_unix(char *s)
{
    int year, month, day, hour, min, sec;
    if (parse(&s, "%4d%2d%2d%2d%2d%2d", &year, &month, &day, &hour, &min, &sec))
        return datetime_to_unix(year, month, day, hour, min, sec);
    else
        return -1;
}

int unix_to_datetime(int unixtime, int *oyear, int *omonth, int *oday, int *hour, int *min, int *sec)
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
        for (month = 1; days_since_year >= dpmleap[month]; ++month)
            days_since_year -= dpmleap[month];
    } else {
        for (month = 1; days_since_year >= dpm[month]; ++month)
            days_since_year -= dpm[month];
    }

    *oyear = year;
    *omonth = month;
    *oday = days_since_year + 1;

    *hour = hour_of_day;
    *min = min_of_hour;
    *sec = sec_of_min;
    return 0;
}

int unix_to_sdatetime(int unixtime, char *s)
{
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int sta = unix_to_datetime(unixtime, &year, &month, &day, &hour, &min, &sec);
    if (sta < 0)
        return -1;

#ifdef DEBUGTIM
    nk_printf("%d-%d-%d %d:%d:%d\n", year, month, day, hour, min, sec);
#endif

    nk_snprintf(s, 40, "%4.4d%2.2d%2.2d%2.2d%2.2d%2.2d", year, month, day, hour, min, sec);
    return 0;
}

#ifdef DEBUGTIM
int main(int argc, char *argv[])
{
    char buf[15];
    if (!strcmp(argv[1], "unix")) {
        int unixtime = atoi(argv[2]);
        unix_to_sdatetime(unixtime, buf);
        nk_printf("%s\n", buf);
    } else {
        int unixtime = sdatetime_to_unix(argv[2]);
        nk_printf("%d\n", unixtime);
    }
}
#endif
