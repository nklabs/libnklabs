// MCU abstraction layer

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

#ifndef _Inkarch_stm32
#define _Inkarch_stm32

#include <stdint.h>

// PICORV32 Interrupt bits
#define TIMER_IRQ (1 << 0)
#define EBREAK_IRQ (1 << 1)
#define BUS_ERROR_IRQ (1 << 2)
#define UART_IRQ (1 << 3)
#define FPGA_IRQ (1 << 4)
#define IRQ_5 (1 << 5)

// PICORV32 registers
#define reg_spictrl (*(volatile uint32_t*)0x02000000)
#define reg_uart_clkdiv (*(volatile uint32_t*)0x02000004)
#define reg_uart_data (*(volatile uint32_t*)0x02000008)
#define reg_leds (*(volatile uint32_t*)0x0200000c)
#define reg_cpu_freq (*(volatile uint32_t*)0x02000010)
#define reg_wallclock (*(volatile uint32_t*)0x02000014)


// Replace IRQ mask with new_mask
//  An interrupt is masked if its bit is high
//  The original mask is returned in old_mask
#define picorv32_maskirq(old_mask, new_mask) \
    __asm__ __volatile__ (".insn r 11, 6, 3, %0, %1, zero\n" : "=r" (old_mask) : "r" (new_mask))

// Replace timer counter with new_count
// The original counter is returned in old_count
// A count of zero means disable timer
// An interrupt is generated when the timer transitions from 1 to 0.
#define picorv32_timer(old_count, new_count) \
    __asm__ __volatile__ (".insn r 11, 6, 5, %0, %1, zero\n" : "=r" (old_count) : "r" (new_count))

// Wait for an interrupt
// When an interrupt occurs, the bit-list of pending interrupts is returned in pending
#define picorv32_waitirq(pending) \
    __asm__ __volatile__ (".insn r 11, 4, 4, %0, 0, 0\n" : "=r" (pending))

// Borrow Linux kernel lock syntax

typedef int spinlock_t;
#define SPIN_LOCK_UNLOCKED 0

// Restore interrupt enable flag
#define nk_irq_unlock(lock, flags) \
    do { \
            uint32_t junk; \
            picorv32_maskirq(junk, flags); \
    } while (0);

// Save interrupt enable flag and disable all interrupts
#define nk_irq_lock(lock, flags) \
    do { \
            picorv32_maskirq(flags, ~0); \
    } while (0);

#define nk_irq_unlock_and_wait(lock, flags, deepness) \
    do { \
        nk_irq_unlock(lock, flags); \
    } while (0);


// Scheduler timer

typedef uint32_t nk_time_t;

// Units for scheduler time
#define NK_TIME_COUNTS_PER_SECOND 25000000
#define NK_TIME_COUNTS_PER_USECOND 25

// Get current time
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

// Microsecond delay

void nk_udelay(unsigned long usec);

// Main interrupt handler

uint32_t *irq(uint32_t *regs, uint32_t irqs);

void picorv32_timer_irq();

#endif
