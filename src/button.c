#include "nkarch.h"
#include "nksched.h"
#include "nkprintf.h"

#define BUTTON_POLL_DELAY 16
#define BUTTON_DEBOUNCE_COUNT 1

int button_tid;
int button_old;
volatile int button_debounce_count;

void button_poll()
{
    // Read current button level
    int down = !gpio_get_pin_level(USER_BUTTON);

    // Edge detector
    if (down && !button_old)
    {
        nk_printf("Pressed!\n");
    }
    else if (!down && button_old)
    {
        // nk_printf("Released!\n");
    }
    button_old = down;

    // Poll again later
    nk_sched(button_tid, button_poll, NULL, BUTTON_POLL_DELAY, "Button poller");

    // Allow change interrupt to trigger a sooner poll, but only
    // after the debounce count has expired
    if (button_debounce_count)
    {
        --button_debounce_count;
    }
    else
    {
        ext_irq_enable(USER_BUTTON);
    }
}

void button_int(void)
{
    // Turn off interrupts to prevent interrupt storm from contact noise
    ext_irq_disable(USER_BUTTON);

    // They will be re-enabled after (BUTTON_DEBOUNCE_COUNT * BUTTON_POLL_DELAY) ms
    button_debounce_count = BUTTON_DEBOUNCE_COUNT;

    // Wake up poller
    nk_sched(button_tid, button_poll, NULL, 0, "Button poller");
}

void nk_init_button()
{
    startup("Button\n");
    button_tid = nk_alloc_tid();
    nk_sched(button_tid, button_poll, NULL, BUTTON_POLL_DELAY, "Button poller");
    button_debounce_count = BUTTON_DEBOUNCE_COUNT;
    ext_irq_register(USER_BUTTON, button_int);
} 
