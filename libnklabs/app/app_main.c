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

// Demo application startup code

#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkdbase.h"
#include "database.h"
#include "button.h"
#include "led.h"
#include "startup.h"
#include "wdt.h"
#ifdef NK_PLATFORM_ATSAM
#include "nkdriver_rtc_atsam.h"
#endif

extern void extrtc_init(void);

// SPI transfer buffer
uint8_t spi_buffer[132];

void user_main(void)
{
#ifdef NK_PLATFORM_STM32
    // Record bootup reason
    reset_cause = reset_cause_get();
#endif

#if 0
    // Show we're alive even if uart isn't working
    int x;
    for (x = 0; x != 5;++x)
    {
        gpio_set_pin_level(USER_LED, false);
        nk_udelay(300000);
        gpio_set_pin_level(USER_LED, true);
        nk_udelay(300000);
    }
#endif

    nk_init_uart();
    nk_init_sched();
#ifdef NK_PLATFORM_ATSAM
    nk_mcu_rtc_init(&CALENDAR_0);
#endif
    nk_init_cli();
    database_init();
#ifdef NK_PLATFORM_ATSAM
    spi_m_sync_enable(&ARD_SPI);
#ifdef ALT_SPI
    spi_m_sync_enable(&ALT_SPI);
#endif
#endif
    nk_init_led();
    nk_init_button();
    nk_init_wdt();

    // Go
    nk_sched_loop();
}

#ifdef NK_PLATFORM_ATSAM

int main(void)
{
    atmel_start_init();
    user_main();
}

#endif
