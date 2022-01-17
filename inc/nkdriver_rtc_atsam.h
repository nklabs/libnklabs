#ifndef _Inkdriver_rtc_atsam
#define _Inkdriver_rtc_atsam

#include "nkarch.h"
#include "nkdatetime.h"

// Interface to on-die MCU RTC
int nk_mcu_rtc_init(struct calendar_descriptor *port);
int nk_mcu_rtc_set_datetime(struct calendar_descriptor *port, const nkdatetime_t *datetime);
int nk_mcu_rtc_get_datetime(struct calendar_descriptor *port, nkdatetime_t *datetime);

#endif
