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

#ifndef _Inkarch_stm32
#define _Inkarch_stm32

// GPIO pins and devices, also includes stm32g0xx_hal.h
#include "main.h"

// Borrow Linux kernel lock syntax

typedef int spinlock_t;
#define SPIN_LOCK_UNLOCKED 0

// Restore interrupt enable flag
#define nk_irq_unlock(lock, flags) \
    do { \
        if (flags) \
            __enable_irq(); \
    } while (0);

// Save interrupt enable flag and disable interrupts
#define nk_irq_lock(lock, flags) \
    do { \
        flags = !(1 & __get_PRIMASK()); \
        __disable_irq(); \
    } while (0);

#define nk_irq_unlock_and_wait(lock, flags, deepness) \
    do { \
        switch (deepness) { \
            case 1: { \
                HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI); \
                nk_irq_unlock(lock, flags); \
            } case 2: { \
                HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI); \
                nk_irq_unlock(lock, flags); \
            } default: { \
                nk_irq_unlock(lock, flags); \
                break; \
            } \
        } \
    } while (0);

// Scheduler timer

typedef uint32_t nk_time_t;

// Current time
nk_time_t nk_get_time();

// Convert milliseconds into scheduler time
nk_time_t nk_convert_delay(uint32_t delay);

// Initialize scheduler timer
void nk_init_sched_timer();

// Set an alarm for 'when'.  An interrupt should fire at this time.
// Interrupts will be masked when this is called.

// If when is now or in the past, the interrupt should also fire.

// The system must wake up immediately on the next call to nk_irq_unlock_and_wait.

void nk_sched_wakeup(nk_time_t when);

// Units for wall time
#define NK_TIME_COUNTS_PER_SECOND 1000

// Microsecond delay

extern uint32_t nk_ticks_per_ms;

void nk_udelay(unsigned long usec);

typedef enum reset_cause_e
{
    RESET_CAUSE_UNKNOWN = 0,
    RESET_CAUSE_LOW_POWER_RESET,
    RESET_CAUSE_WINDOW_WATCHDOG_RESET,
    RESET_CAUSE_INDEPENDENT_WATCHDOG_RESET,
    RESET_CAUSE_SOFTWARE_RESET,
    RESET_CAUSE_POWER_ON_POWER_DOWN_RESET,
    RESET_CAUSE_EXTERNAL_RESET_PIN_RESET
} reset_cause_t;

extern reset_cause_t reset_cause;

// Obtain the STM32 system reset cause and clear it
reset_cause_t reset_cause_get(void);

// Convert reset cause to string
const char *reset_cause_get_name(reset_cause_t reset_cause);

// MCU flash padding size
#define NK_MCUFLASH_MIN_SIZE 8

// Start address of flash
#define NK_FLASH_BASE_ADDRESS 0x8000000

// Reboot MCU
void reboot(void);

#endif
