#ifndef _Inkdriver_rtc_stm32
#define _Inkdriver_rtc_stm32

#include "nkarch.h"
#include "nkdatetime.h"

// Interface to on-die MCU RTC
int nk_mcu_rtc_init(RTC_HandleTypeDef *rtc);
int nk_mcu_rtc_set_datetime(RTC_HandleTypeDef *rtc, const nkdatetime_t *datetime);
int nk_mcu_rtc_get_datetime(RTC_HandleTypeDef *rtc, nkdatetime_t *datetime);

#endif
