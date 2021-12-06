#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkrtc.h"
#include "nkdbase.h"
#include "database.h"
#include "startup.h"

int wdt_tid;

void wdt_poke(void *data)
{
    (void)data;
    //HAL_IWDG_Refresh(&hiwdg);
    nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");
}

extern void spiflash_init(void);

int main(void)
{
    atmel_start_init();

#ifdef junk
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
    nk_rtc_init();
    nk_init_cli();
    wdt_tid = nk_alloc_tid();
    nk_sched(wdt_tid, wdt_poke, NULL, 5000, "Watchdog timer poker");
    database_init();
    spiflash_init();
    nk_sched_loop();
}
