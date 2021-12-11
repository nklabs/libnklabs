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

#ifdef IFLASH_ADDR

// atsame70
#include <hpl_efc_config.h>

#define NK_MCUFLASH_BASE (IFLASH_ADDR)
#define NK_MCUFLASH_PAGE_SIZE (CONF_FLASH_PAGE_SIZE)
#define NK_MCUFLASH_ERASE_SIZE (CONF_FLASH_ERASING_BLK_SIZE)
#define NK_MCUFLASH_MIN_SIZE 1
#define NK_MCUFLASH_SIZE (IFLASH_SIZE)

#else

// This is the base address of the flash memory
// atsamd21, atsame54
#define NK_MCUFLASH_BASE 0x00000000

// This is the page size
// atsamd21, atsame54
#define NK_MCUFLASH_PAGE_SIZE (NVMCTRL_PAGE_SIZE)

// This is the erase granularity
#ifdef NVMCTRL_ROW_PAGES

// atsamd21
#define NK_MCUFLASH_ERASE_SIZE (NK_MCUFLASH_PAGE_SIZE * NVMCTRL_ROW_PAGES)

#else

// atsame54
#define NK_MCUFLASH_ERASE_SIZE NVMCTRL_BLOCK_SIZE

#endif

// This is the minimum write size
#define NK_MCUFLASH_MIN_SIZE 1

// Size of flash memory
#ifdef FLASH_NB_OF_PAGES

// atsamd21
#define NK_MCUFLASH_SIZE (NK_MCUFLASH_PAGE_SIZE * FLASH_NB_OF_PAGES)

#else

// atsame54 - lame, not available at compile time
#define NK_MCUFLASH_SIZE (NK_MCUFLASH_PAGE_SIZE * (uint32_t)hri_nvmctrl_read_PARAM_NVMP_bf(FLASH_0.hw))

#endif

#endif

void reboot(void);

#endif
