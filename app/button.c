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

// Print a message when a button is pressed using interrupts
// (also button release is detected)

// The button GPIO is polled for reliability, but the button interrupt
// handler reschedules the polling task for immediate execution, thereby
// reducing the button press latency.  Interrupt is then disabled for a
// while to eliminate possible interrupt storm from contact noise.

// GPIO interrupt should be set to fire on either detected edge.

#include "nkarch.h"
#include "nksched.h"
#include "nkprintf.h"

// Annoying that these are not included by default..

#if defined(STM32F030x8)
#include "stm32f0xx_ll_exti.h"
#endif

#if defined(STM32F103xB)
#include "stm32f1xx_ll_exti.h"
#endif

#if defined(STM32G070xx) || defined(STM32G071xx)
#include "stm32g0xx_ll_exti.h"
#endif

#if defined(STM32L432xx)
#include "stm32l4xx_ll_exti.h"
#endif

#if defined(STM32F746xx)
#include "stm32f7xx_ll_exti.h"
#endif

#if defined(STM32L073xx)
#include "stm32l0xx_ll_exti.h"
#endif

#define BUTTON_POLL_DELAY 100 // Polling interval
#define BUTTON_DEBOUNCE_COUNT 1 // How many intervals to leave interrupts off for debouncing

int button_tid; // Task ID for button
int button_old; // Previous state of button for edge detector
volatile int button_debounce_count; // Debounce counter

// Read button GPIO, look for edge

void button_poll()
{
    int down = 0;
    // Read current button level
#ifdef USER_BUTTON_Pin
    // STM32
#if defined(STM32F746xx)
    down = !!HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
#else
    down = !HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
#endif

#endif



#ifdef USER_BUTTON
    // ATSAM
    down = !gpio_get_pin_level(USER_BUTTON);
#endif

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
#ifdef USER_BUTTON_Pin
        // STM32
        LL_EXTI_EnableIT_0_31(USER_BUTTON_Pin);
#endif

#ifdef USER_BUTTON
        // ATSAM
        ext_irq_enable(USER_BUTTON);
#endif
    }
}

// Button edge interrupt handler

#ifdef USER_BUTTON_Pin

// STM32

void gpio_exti_irq_handler(uint16_t gpio_pin)
{
    if (gpio_pin == USER_BUTTON_Pin)
    {
        // Disable interrupt from just this pin
        LL_EXTI_DisableIT_0_31(USER_BUTTON_Pin);

        // They will be re-enabled after (BUTTON_DEBOUNCE_COUNT * BUTTON_POLL_DELAY) ms
        button_debounce_count = BUTTON_DEBOUNCE_COUNT;

        // Wake up poller
        nk_sched(button_tid, button_poll, NULL, 0, "Button poller");

        //nk_printf("IRQ! %d\n", gpio_pin);
    }
}

// Older ones have this

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
    gpio_exti_irq_handler(gpio_pin);
}

// Newer ones have these

void HAL_GPIO_EXTI_Rising_Callback(uint16_t gpio_pin)
{
    gpio_exti_irq_handler(gpio_pin);
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t gpio_pin)
{
    gpio_exti_irq_handler(gpio_pin);
}



#endif

#ifdef USER_BUTTON

// ATSAM

void button_int(void)
{
    // Turn off interrupts to prevent interrupt storm from contact noise
    ext_irq_disable(USER_BUTTON);

    // They will be re-enabled after (BUTTON_DEBOUNCE_COUNT * BUTTON_POLL_DELAY) ms
    button_debounce_count = BUTTON_DEBOUNCE_COUNT;

    // Wake up poller
    nk_sched(button_tid, button_poll, NULL, 0, "Button poller");
}

#endif

// Initialize used button

void nk_init_button()
{
    nk_startup_message("Button\n");

    // Start poller
    button_tid = nk_alloc_tid();
    nk_sched(button_tid, button_poll, NULL, BUTTON_POLL_DELAY, "Button poller");
    button_debounce_count = BUTTON_DEBOUNCE_COUNT;

#ifdef USER_BUTTON
    // ATSAM- register interrupt handler
    ext_irq_register(USER_BUTTON, button_int);
#endif
} 
