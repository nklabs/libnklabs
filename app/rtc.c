#include "nkarch.h"

#ifdef NK_PLATFORM_STM32
#include "nkdriver_rtc_stm32.h"
extern RTC_HandleTypeDef *hrtc;
#endif

#ifdef NK_PLATFORM_ATSAM
#include "nkdriver_rtc_atsam.h"
#endif

#include "nkrtc.h"


const nk_rtc_t mcu_rtc = {
    .set_datetime = (int (*)(void *, const nkdatetime_t *))nk_mcu_rtc_set_datetime,
    .get_datetime = (int (*)(void *, nkdatetime_t *))nk_mcu_rtc_get_datetime,
#ifdef NK_PLATFORM_STM32
    .port = &hrtc
#endif
#ifdef NK_PLATFORM_STM32
    .port = &CALENDAR_0
#endif
};

int cmd_rtc(nkinfile_t *args)
{
    return nk_rtc_command(&mcu_rtc, args);
}

COMMAND(cmd_rtc,
	">rtc                       MCU Real Time Clock access\n"
        "-rtc                       Show date/time\n"
        "-rtc YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
