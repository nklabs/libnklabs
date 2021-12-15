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
#ifdef USER_LED_Pin
        // STM32
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
#endif
        led_blink = 0;
    }
    else
    {
#ifdef USER_LED
        // ATSAM
        gpio_set_pin_level(USER_LED, false);
#endif
#ifdef USER_LED_Pin
        // STM32
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
#endif
        led_blink = 1;
    }
    nk_sched(led_tid, led_blinker, NULL, BLINK_DELAY, "LED blinker");
}

void shared_gpio_setup_for_led()
{
#ifdef USER_LED_Pin
    // On most STM32 dev. board, the MAIN_SPI_CLK line is shared with USER_LED
    // Set it up for LED mode
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = USER_LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    led_blink = 0;
#endif
}

void shared_gpio_setup_for_spi()
{
#ifdef USER_LED_Pin
    // Reconfigure GPIOs for SPI
    HAL_SPI_MspInit(&MAIN_SPI);
#endif
}

void nk_init_led()
{
    nk_startup_message("LED\n");
    shared_gpio_setup_for_led();
    led_tid = nk_alloc_tid();
    nk_sched(led_tid, led_blinker, NULL, BLINK_DELAY, "LED blinker");
}
