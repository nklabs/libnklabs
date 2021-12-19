// Demo application startup code

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
    nk_mcu_rtc_init();
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
    extrtc_init();
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
