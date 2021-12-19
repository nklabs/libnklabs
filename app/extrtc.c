#include "nkarch.h"
#include "nkextrtc.h"

int cmd_extrtc(nkinfile_t *args)
{
    return nk_ext_rtc_command(&ARD_I2C, args);
}

COMMAND(extrtc,
	"extrtc                    External RTC access\n",
        "extrtc                    Show date/time\n"
        "extrtc YYYY-MM-DD HH:MM:SS\n"
        "                          Set date/time\n",
        ""
)

void extrtc_init(void)
{
    nk_startup_message("External Real Time Clock\n");
    nk_ext_rtc_init(&ARD_I2C);
}
