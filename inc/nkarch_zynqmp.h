// Copyright 2020 NK Labs, LLC

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

#ifndef   _NKARCH_ZYNQMP_H
#define  _NKARCH_ZYNQMP_H

#include <stdint.h>

#include "platform.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "xuartps.h"
#include "xgpiops.h"
#include "xttcps.h"
#include "xiicps.h"
#include "xtime_l.h"
#include "sleep.h"

// MCU abstraction layer

#define NK_FLASH

// Borrow Linux kernel lock syntax

typedef int nk_spinlock_t;
typedef unsigned long nk_irq_flag_t;
#define SPIN_LOCK_UNLOCKED 0

// Restore interrupt enable flag
static inline __attribute__((always_inline)) void nk_irq_unlock(nk_spinlock_t *lock, nk_irq_flag_t flags)
{
    (void)lock;
    if (flags)
        Xil_ExceptionEnable();
}

// Try to sleep, then restore interrupt enable flag
static inline __attribute__((always_inline)) void nk_irq_unlock_and_wait(nk_spinlock_t *lock, nk_irq_flag_t flags, int deepness)
{
    switch (deepness) {
        case 1: {
            nk_irq_unlock(lock, flags);
            break;
        } case 2: {
            nk_irq_unlock(lock, flags);
            break;
        } default: {
            nk_irq_unlock(lock, flags);
            break;
        }
    }
}

// Save interrupt enable flag and disable all interrupts
// Acquire spinlock on multi-core systems

static inline __attribute__((always_inline)) nk_irq_flag_t nk_irq_lock(nk_spinlock_t *lock)
{
    (void)lock;
    nk_irq_flag_t flags = !(mfcpsr() & XREG_CPSR_IRQ_ENABLE);
    Xil_ExceptionDisable();
    return flags;
}

#define nk_irq_unlock_and_wait(lock, flags, deepness) \
    do { \
        nk_irq_unlock(lock, flags); \
    } while (0);

// Scheduler timer

typedef XTime nk_time_t;

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

#define NK_TIME_COUNTS_PER_USECOND (COUNTS_PER_SECOND/1000000)
#define NK_TIME_COUNTS_PER_SECOND (COUNTS_PER_SECOND)

// Microsecond delay

extern uint32_t nk_ticks_per_ms;

void nk_udelay(unsigned long usec);

// MCU flash padding size
#define NK_MCUFLASH_MIN_SIZE 8

// Start address of flash
#define NK_FLASH_BASE_ADDRESS 0x8000000

// Reboot MCU
void nk_reboot(void);

// ZynqMP interrupt controller

extern XScuGic interrupt_controller;
void iz_intc(void);

#endif    /*  NKARCH_ZYNQMP_H   */
