// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// STM32 built-in RTC

#include <string.h>
#include "nkdriver_rtc_stm32.h"

int nk_mcu_rtc_get_datetime(RTC_HandleTypeDef *rtc, nkdatetime_t *datetime)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(rtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(rtc, &sDate, RTC_FORMAT_BIN);

    datetime->year = sDate.Year + 2000;
    datetime->month = sDate.Month - 1;
    datetime->day = sDate.Date - 1;
    datetime->weekday = sDate.WeekDay - 1;

    datetime->hour = sTime.Hours;
    datetime->min = sTime.Minutes;
    datetime->sec = sTime.Seconds;

    return nk_datetime_sanity(datetime);
}

int nk_mcu_rtc_set_datetime(RTC_HandleTypeDef *rtc, const nkdatetime_t *datetime)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    memset(&sTime, 0, sizeof(sTime));
    memset(&sDate, 0, sizeof(sDate));

    sDate.Year = datetime->year - 2000; // 0..99
    sDate.Month = datetime->month + 1;
    sDate.Date = datetime->day + 1; // 1..31
    sDate.WeekDay = datetime->weekday + 1; // 1..7

    sTime.Hours = datetime->hour;
    sTime.Minutes = datetime->min;
    sTime.Seconds = datetime->sec;

    HAL_RTC_SetTime(rtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(rtc, &sDate, RTC_FORMAT_BIN);
    return 0;
}
