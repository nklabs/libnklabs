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

// Poke watchdog timer

#include "nkarch.h"
#include "nksched.h"
#include "nkprintf.h"
#include "wdt.h"

#define WDT_DELAY 5000

int wdt_tid;

void wdt_poke(void *data)
{
    (void)data;
#ifdef NK_PLATFORM_ATSAM
    wdt_feed(&MAIN_WDT);
#endif
#ifdef NK_PLATFORM_STM32
    HAL_IWDG_Refresh(&MAIN_WDT);
#endif
    nk_sched(wdt_tid, wdt_poke, NULL, WDT_DELAY, "Watchdog timer poker");
}

void nk_init_wdt(void)
{
    nk_startup_message("WDT\n");
#ifdef NK_PLATFORM_ATSAM
    wdt_enable(&MAIN_WDT);
#endif
    wdt_tid = nk_alloc_tid();
    nk_sched(wdt_tid, wdt_poke, NULL, 1, "Watchdog timer poker");
}
