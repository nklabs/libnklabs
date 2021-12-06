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

#ifndef _Inkarch_atsam
#define _Inkarch_atsam

#include <atmel_start.h>

// Borrow Linux kernel lock syntax

typedef int spinlock_t;
#define SPIN_LOCK_UNLOCKED 0

// Restore interrupt enable flag
#define nk_irq_unlock(lock, flags) \
    do { \
        __set_PRIMASK(flags); \
    } while (0);

// Save interrupt enable flag and disable all interrupts
#define nk_irq_lock(lock, flags) \
    do { \
        flags = __get_PRIMASK(); \
        __disable_irq(); \
    } while (0);

#define nk_irq_unlock_and_wait(lock, flags, deepness) \
    do { \
        __set_PRIMASK(flags); \
    } while (0);


// Scheduler timer

uint32_t nk_get_sched_time();
uint32_t nk_get_sched_timeout();
uint32_t nk_convert_delay(uint32_t delay);
void nk_init_sched_timer();
void nk_start_sched_timer(uint32_t delay);

// Wall time

typedef uint32_t nk_time_t;
nk_time_t nk_get_time();

// Units for wall time
#define NK_TIME_COUNTS_PER_SECOND 1000
#define NK_TIME_COUNTS_PER_USECOND 1

// Microsecond delay

void nk_udelay(unsigned long usec);

// Get sector size.  This is the minimum size that nk_flash_erase can erase.
//uint32_t nk_flash_sector_size();
#define NK_MCUFLASH_ERASE_SECTOR_SIZE 8192

void reboot(void);

#endif
