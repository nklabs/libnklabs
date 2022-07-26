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

// MCU abstraction layer

// Borrow Linux kernel lock syntax

typedef int spinlock_t;
#define SPIN_LOCK_UNLOCKED 0

// Restore interrupt enable flag
#define nk_irq_unlock(lock, flags) \
    do { \
        __asm__ __volatile__ ("" ::: "memory"); /* Prevent C compiler from caching memory through locks */ \
        if (flags) \
            /* __enable_irq(); */ \
            /* am_hal_interrupt_master_enable(); */ \
            Xil_ExceptionEnable(); \
    } while (0);

// Save interrupt enable flag and disable interrupts
#define nk_irq_lock(lock, flags) \
    do { \
        __asm__ __volatile__ ("" ::: "memory"); /* Prevent C compiler from caching memory through locks */ \
        /* flags = !(1 & __get_PRIMASK()); */ \
        /* __disable_irq(); */ \
        /* flags = !am_hal_interrupt_master_disable(); */ \
        flags = !(mfcpsr() & XREG_CPSR_IRQ_ENABLE); \
        Xil_ExceptionDisable(); \
    } while (0);

#define nk_irq_unlock_and_wait(lock, flags, deepness) \
    do { \
        nk_irq_unlock(lock, flags); \
    } while (0);

// Scheduler timer

uint32_t nk_get_sched_time();
uint32_t nk_get_sched_timeout();
uint32_t nk_convert_delay(uint32_t delay);
void nk_init_sched_timer();
void nk_start_sched_timer(uint32_t delay);

// Wall time

typedef XTime nk_time_t;
nk_time_t nk_get_time();

// Units for wall time

#define NK_TIME_COUNTS_PER_USECOND (COUNTS_PER_SECOND/1000000)
#define NK_TIME_COUNTS_PER_SECOND (COUNTS_PER_SECOND)

// Microsecond delay

void nk_udelay(unsigned long usec);

#endif    /*  NKARCH_ZYNQMP_H   */
