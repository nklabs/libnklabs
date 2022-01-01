#include "nkarch.h"
#include "nkmcurtc.h"
#include "nkextrtc.h"

int cmd_extrtc(nkinfile_t *args)
{
    return nk_ext_rtc_command(&ARD_I2C, args);
}

COMMAND(cmd_extrtc,
	">extrtc                    External RTC access\n"
        "-extrtc                    Show date/time\n"
        "-extrtc YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)

void extrtc_init(void)
{
    nkdatetime_t datetime;
    nk_startup_message("External Real Time Clock\n");
    nk_ext_rtc_init(&ARD_I2C);
    int rtn = nk_ext_rtc_get_datetime(&ARD_I2C, &datetime);
    if (rtn == NK_ERROR_TIME_LOST)
    {
        nk_printf("  Time lost\n");
    }
    else if (rtn)
    {
        nk_printf("  Error accessing Real Time Clock\n");
    }
    else
    {
        nk_printf("  Setting MCU RTC\n");
        nk_mcu_rtc_set_datetime(&datetime);
    }
}
