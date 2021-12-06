#include "nkarch.h"
#include "nkcli.h"
#include "nkrtc.h"

int ext_rtc_set_time(int year, int month, int day, int hour, int min, int sec)
{
    // ATSAM
    struct calendar_date_time tim;
    int rtn;

    tim.time.sec = sec;
    tim.time.min = min;
    tim.time.hour = hour;
    tim.date.day = day;
    tim.date.month = month;
    tim.date.year = year;

    rtn = calendar_set_date(&CALENDAR_0, &tim.date);
    rtn = calendar_set_time(&CALENDAR_0, &tim.time);

    return rtn;
}

int ext_rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    // ATSAM
    struct calendar_date_time tim;
    int rtn;

    rtn = calendar_get_date_time(&CALENDAR_0, &tim);

    *sec = tim.time.sec;
    *min = tim.time.min;
    *hour = tim.time.hour;
    *day = tim.date.day;
    *month = tim.date.month;
    *year = tim.date.year;

    return rtn;
}

void nk_rtc_init()
{
    startup("Real time clock\n");
    calendar_enable(&CALENDAR_0);
}

// Internal RTC

int cmd_date(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d %e", &year, &month, &day, &hour, &min, &sec)) {
        int rtn = ext_rtc_set_time(year, month, day, hour, min, sec);
        if (rtn) {
            nk_printf("Error %d\n", rtn);
        } else {
            nk_printf("Date set.\n");
        }
    } else if (nk_fscan(args, "%e")) {
        int rtn = ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
        if (rtn) {
            nk_printf("Error %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(date,
	"date                      RTC access\n",
        "date                      Show date/time\n"
        "date YYYY-MM-DD HH:MM:SS  Set date/time\n",
        ""
)

void date_string(char *buf)
{
    int year, month, day, hour, min, sec;
    year = 2000;
    month = 1;
    day = 1;
    hour = 0;
    min = 0;
    sec = 0;
    ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
    nk_snprintf(buf, 40, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ", year, month, day, hour, min, sec);
}
