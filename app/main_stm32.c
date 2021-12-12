// Demo application startup code

#include "nkarch.h"
#include "nkuart.h"
#include "nksched.h"
#include "nkcli.h"
#include "nkdbase.h"
#include "database.h"
#include "button.h"
#include "led.h"
#include "wdt.h"

// Main for application

void user_main(void)
{
    // Record bootup reason
    reset_cause = reset_cause_get();

    // Initialize
    nk_init_uart();
    nk_init_sched();
    nk_init_cli();
    database_init();
    nk_init_led();
    nk_init_button();
    nk_init_wdt();

    // Go
    nk_sched_loop();
}
