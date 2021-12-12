#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkmcurtc.h"
#include "nkdbase.h"
#include "database.h"
#include "button.h"
#include "led.h"
#include "startup.h"
#include "wdt.h"

int test_tid;

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
    nk_mcu_rtc_init();
    nk_init_cli();
    database_init();
    spiflash_init();
    nk_init_led();
    nk_init_button();
    nk_init_wdt();
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
