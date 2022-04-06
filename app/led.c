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

#include "nkarch.h"
#include "nksched.h"
#include "nkprintf.h"
#include "led.h"

// Blink USER LED

#define BLINK_DELAY 500 // Blink interval

int led_tid; // Task ID for LED blinker
int led_blink; // LED state

// Blink USER_LED

void led_blinker(void *data)
{
    (void)data;
    if (led_blink)
    {
#ifdef USER_LED
        // ATSAM
        gpio_set_pin_level(USER_LED, true);
#endif
#ifdef ARD_D13_Pin
        // STM32
        HAL_GPIO_WritePin(ARD_D13_GPIO_Port, ARD_D13_Pin, GPIO_PIN_SET);
#endif
        led_blink = 0;
    }
    else
    {
#ifdef USER_LED
        // ATSAM
        gpio_set_pin_level(USER_LED, false);
#endif
#ifdef ARD_D13_Pin
        // STM32
        HAL_GPIO_WritePin(ARD_D13_GPIO_Port, ARD_D13_Pin, GPIO_PIN_RESET);
#endif
        led_blink = 1;
    }
    nk_sched(led_tid, led_blinker, NULL, BLINK_DELAY, "LED blinker");
}

void shared_gpio_setup_for_led()
{
#ifdef ARD_D13_Pin
    // On most STM32 dev. board, the ARD_SPI_CLK line is shared with USER_LED
    // Set it up for LED mode
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = ARD_D13_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(ARD_D13_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(ARD_D13_GPIO_Port, ARD_D13_Pin, GPIO_PIN_SET);
    led_blink = 0;
#endif
}

void shared_gpio_setup_for_spi()
{
#ifdef ARD_D13_Pin
    // Reconfigure GPIOs for SPI
    HAL_SPI_MspInit(&ARD_SPI);
#endif
}

void nk_init_led()
{
    nk_startup_message("LED\n");
    shared_gpio_setup_for_led();
    led_tid = nk_alloc_tid();
    nk_sched(led_tid, led_blinker, NULL, BLINK_DELAY, "LED blinker");
}
