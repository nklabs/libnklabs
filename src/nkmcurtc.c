// MCU Real Time Clock

#include "nkarch.h"
#include "nkcli.h"
#include "nkmcurtc.h"

const char *nk_day_name[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

// Compute day of week from date

int nk_calc_day_of_week(int y, int m, int d)
{
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= m < 3;
    return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

// Return current minute of day

int nk_mcu_rtc_get_minute_of_day()
{
    int year, month, day;
    int hour, min, sec;
    nk_mcu_rtc_get_datetime(&year, &month, &day, &hour, &min, &sec);
    return hour * 60 + min;
}

// Return current minute of week

int nk_mcu_get_minute_of_week()
{
    int year;
    int month;
    int day;
    int weekday;
    int hour;
    int min;
    int sec;
    nk_mcu_rtc_get_datetime(&year, &month, &day, &hour, &min, &sec);
    weekday = nk_calc_day_of_week(year, month, day);
    return weekday * 1440 + (hour * 60) + min;
}

// Get date/time as a string: YYYY-MM-DD HH:MM:SS

int nk_mcu_rtc_date_string(char *buf, size_t len)
{
    int rtn;
    int year, month, day, hour, min, sec;
    year = 2000;
    month = 1;
    day = 1;
    hour = 0;
    min = 0;
    sec = 0;
    rtn = nk_mcu_rtc_get_datetime(&year, &month, &day, &hour, &min, &sec);
    nk_snprintf(buf, len, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d", year, month, day, hour, min, sec);
    return rtn;
}

// User interface

int cmd_mcurtc(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d ", &year, &month, &day, &hour, &min, &sec)) {
        int rtn = nk_mcu_rtc_set_datetime(year, month, day, hour, min, sec);
        if (rtn) {
            nk_printf("Error getting date/time %d\n", rtn);
        } else {
            nk_printf("Time and date set.\n");
        }
    } else if (nk_fscan(args, "%e")) {
        int rtn = nk_mcu_rtc_get_datetime(&year, &month, &day, &hour, &min, &sec);
        if (rtn) {
            nk_printf("Error setting date/time %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", year, month, day, hour, min, sec);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(mcurtc,
	"mcurtc                    RTC access\n",
        "mcurtc                    Show date/time\n"
        "mcurtc YYYY-MM-DD HH:MM:SS\n"
        "                          Set date/time\n",
        ""
)
