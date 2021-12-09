// Demo application startup code

#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkdbase.h"
#include "database.h"

// Watchdog timer

int wdt_tid;

void wdt_poke(void *data)
{
    (void)data;
    HAL_IWDG_Refresh(&MAIN_WDT);
    nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");
}

// Main for application

void user_main(void)
{
    // Record bootup reason
    reset_cause = reset_cause_get();

    // Initialize
    nk_init_uart();

    nk_init_sched();

    nk_init_cli();

    // Watchdog timer
    wdt_tid = nk_alloc_tid();
    nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");

    database_init();

    // Go
    nk_sched_loop();
}
