#ifndef _Inkarch_stm32
#define _Inkarch_stm32

// MCU abstraction layer

#include "stm32g0xx_hal.h"

// GPIO pins and devices

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

// Microsecond delay

void nk_udelay(unsigned long usec);

// Which I2C slave?

//#define i2c_slave_port hi2c1
//extern I2C_HandleTypeDef i2c_slave_port;

// Which I2C master?

//#define i2c_master_port hi2c2
//extern I2C_HandleTypeDef i2c_master_port;

// SPI master

#define spi1_port hspi1
extern SPI_HandleTypeDef hspi1;

void reboot();

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

#endif
