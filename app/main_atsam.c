#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkrtc.h"
#include "nkdbase.h"
#include "database.h"
#include "button.h"
#include "startup.h"

int test_tid;

// Poke watchdog timer, blink USER_LED

#define BLINK_DELAY 500

int wdt_tid;
int led_blink;

void wdt_poke(void *data)
{
    (void)data;
    wdt_feed(&MAIN_WDT);
    if (led_blink)
    {
        gpio_set_pin_level(USER_LED, true);
        led_blink = 0;
    }
    else
    {
        gpio_set_pin_level(USER_LED, false);
        led_blink = 1;
    }
    nk_sched(wdt_tid, wdt_poke, NULL, BLINK_DELAY, "Watchdog timer poker");
}

extern void spiflash_init(void);

int main(void)
{
    atmel_start_init();

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
    nk_rtc_init();
    nk_init_cli();
    wdt_tid = nk_alloc_tid();
    nk_sched(wdt_tid, wdt_poke, NULL, BLINK_DELAY, "Watchdog timer poker");
    database_init();
    spiflash_init();
    nk_init_button();
    test_tid = nk_alloc_tid();
    nk_sched_loop();
}

void test_event(void *data)
{
    nk_printf("Ping!\n");
}

static int cmd_test(nkinfile_t *args)
{
    nk_sched(test_tid, test_event, NULL, 100, "Test");
    return 0;
}

COMMAND(test,
    "test                      Test event\n",
    "test                      Test event\n",
    ""
)
