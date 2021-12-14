// MCU Real Time Clock

#include "nkarch.h"
#include "nkcli.h"
#include "nkmcurtc.h"

// User interface

int cmd_mcurtc(nkinfile_t *args)
{
    nkdatetime_t datetime;
    if (nk_fscan(args, "%hd-%hhd-%hhd %hhd:%hhd:%hhd ", &datetime.year, &datetime.month, &datetime.day, &datetime.hour, &datetime.min, &datetime.sec)) {
        datetime.month--;
        datetime.day--;
        datetime.weekday = nk_calc_day_of_week(datetime.year, datetime.month, datetime.day);
        nk_printf("%s\n", nk_weekday_name[datetime.weekday]);
        if (nk_datetime_sanity(&datetime))
        {
            nk_printf("Invalid date/time\n");
        }
        else
        {
            int rtn = nk_mcu_rtc_set_datetime(&datetime);
            if (rtn) {
                nk_printf("Error setting date/time %d\n", rtn);
            } else {
                nk_printf("Time and date set.\n");
            }
        }
    } else if (nk_fscan(args, "%e")) {
        int rtn = nk_mcu_rtc_get_datetime(&datetime);
        if (rtn) {
            nk_printf("Error getting date/time %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", datetime.year, 1 + datetime.month, 1 + datetime.day, datetime.hour, datetime.min, datetime.sec);
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
