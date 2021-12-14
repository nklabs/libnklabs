#include "nkprintf.h"
#include "nkdatetime.h"

const char *nk_weekday_name[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char *nk_month_name[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

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

int nk_mcu_rtc_get_minute_of_day(const nkdatetime_t *datetime)
{
    return datetime->hour * 60 + datetime->min;
}

// Return current minute of week

int nk_mcu_get_minute_of_week(const nkdatetime_t *datetime)
{
    return datetime->weekday * 1440 + (datetime->hour * 60) + datetime->min;
}

// Get date/time as a string: YYYY-MM-DD HH:MM:SS

int nk_mcu_rtc_date_string(char *buf, size_t len, const nkdatetime_t *datetime)
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
    if (datetime->year > 2199 || datetime->year < 2000)
    {
        datetime->year = 2000;
        rtn = -1;
    }
    return rtn;
}
