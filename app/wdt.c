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
