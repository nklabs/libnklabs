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

#include "nkarch.h"

#ifdef NK_PLATFORM_STM32
#include "nkdriver_rtc_stm32.h"
extern RTC_HandleTypeDef hrtc;
#endif

#ifdef NK_PLATFORM_ATSAM
#include "nkdriver_rtc_atsam.h"
#endif

#include "nkrtc.h"


const nk_rtc_t mcu_rtc = {
    .set_datetime = (int (*)(const void *, const nkdatetime_t *))nk_mcu_rtc_set_datetime,
    .get_datetime = (int (*)(const void *, nkdatetime_t *))nk_mcu_rtc_get_datetime,
#ifdef NK_PLATFORM_STM32
    .port = &hrtc
#endif
#ifdef NK_PLATFORM_ATSAM
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
