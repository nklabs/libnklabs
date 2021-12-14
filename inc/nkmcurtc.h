#ifndef _Inkrtc
#define _Inkrtc

#include "nkdatetime.h"

// Interface to on-die MCU RTC
int nk_mcu_rtc_init(void);
int nk_mcu_rtc_set_datetime(const nkdatetime_t *datetime);
int nk_mcu_rtc_get_datetime(nkdatetime_t *datetime);

#endif
