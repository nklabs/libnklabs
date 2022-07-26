# nk_arch: Architecture specific functions

These functions abstract away basic aspects of the specific processor architecture. 
These functions must all be provided when porting to a new processor.

## Files

[nkarch.h](../inc/nkarch.h),
[nkarch_config.h](../config_stm32/nkarch_config.h),
[nkarch_atsam.h](../inc/nkarch_atsam.h),
[nkarch_stm32.h](../inc/nkarch_stm32.h),
[nkarch_atsam.c](../src/nkarch_atsam.c),
[nkarch_stm32.c](../src/nkarch_stm32.c)

Usually you just include nkarch.h, which just includes nkarch_config.h.
nkarch_config.h includes the correct nkarch_xxx.h for the target device.

## Interrutps and sleeping

### nk_irq_lock()

```c
void nk_irq_lock(nk_spinlock_t *spinlock, nk_int &flags);
```

Lock a spinlock and disable interrupts.  The interrupt flags are stashed
in 'flags' so that they may be restored when the lock is released.

It is intended that this function can be direcly implemented by the
spin_lock_irqsave macro in the Linux kernel.  On single-core embedded
systems, this just has to disable interrupts.

### nk_irq_unlock()

```c
void nk_irq_unlock(nk_spinlock_t *spinlock, nk_int flags);
````

Release a spinlock and restore interrupts.

### nk_irq_lock, nk_irq_unlock example

This example shows how to write functions which implement a stack, but
which are safe to call from both interrupt handlers, and non-interrupt
handler code.

```c
int len;
int list[10];
nk_spinlock_t list_lock = NK_SPIN_LOCK_UNLOCKED;

int list_push(int i)
{
	nk_int_t irq_flag;
	nk_irq_lock(&list_lock, irq_flag);
	int status;
	if (len != 10) {
		list[len++] = i;
		status = 0;
	} else {
		status = -1;
	}
	nk_irq_unlock(&list_lock, irq_flag);
	return status;
}

int list_pop(int *val)
{
	int status;
	nk_int_t irq_flag;
	nk_irq_lock(&list_lock, irq_flag);
	if (len) {
		*val = list[--len];
		status = 0;
	} else {
		status = -1;
	}
	nk_irq_unlock(&list_lock, irq_flag);
	return status;
}
```

### nk_irq_unlock_and_wait()

```c
void nk_irq_unlock_and_wait(nk_spinlock_t *spinlock, nk_int_t enable_state, int deepness);
```

Unlock spinlock, restore interrupts and sleep until there is a pending interrupt.

The processor should sleep until there are any pending interrupts, then wake
up and return.  'deepness' indicates how deep the sleep is.  Higher numbers
indicate a deeper sleep.  0 means no sleeping and the function just
immediately returns.

A deep sleep saves power, but typically incurs a higher latency when waking up.

This function is called by the nksched module when there is no work to do.

How this function is implemented depends on the processor.  For example, on
some processors you must sleep while leaving the global interrupt enable bit
disabled.  With these, the processor wakes up if there are pending interrupts
even if they are globally masked.  Other processors might require enabling
before sleeping, but they will need some atomic way of combining the enable
and sleep to avoid the case of ever sleeping after an interrupt is received.

On return, interrupts should be in their restored state just as if
nk_irq_unlock had been called.

## sched_timer: Scheduler Timer

These functions are called by the nksched module and must implement the
timer used by the scheduler.  This timer runs even when the CPU is sleeping.

The idea is to maximize the amount of time the CPU is asleep.  So the
scheduler determines when the CPU must next wake up and starts this timer
with the required delay and then calls nk_irq_unlock_and_wait() in a loop
until nk_get_time() indicates that the requested delay has passed.  A loop
is used because other interrupts may also wake up the CPU.

### nk_init_sched_timer()

```c
void nk_init_sched_timer();
```
Initialize the scheduler timer module.  This is called by nk_init_sched().

### nk_convert_delay()

```c
nk_time_t nk_convert_delay(uint32_t delay)
```

Convert delay in milliseconds into system dependent number of timer ticks. 
For example, if hardware timer runs at 50 Hz, then nk_convert_delay(1000)
returns a value of 20.

### nk_sched_wakeup()

```c
void nk_sched_wakeup(nk_time_t when)
```

Start the scheduler timer.  When the current time exceeds 'when' (measured
in ticks) an interrupt occurs which wakes up the processor (causing
nk_irq_unlock_and_wait() to return).

If 'when' is now or in the past, the interrupt should fire immediately so
that the system wakes up on the next call to nk_irq_unlock_and_wait().

### nk_get_time()

```c
nk_time_t nk_get_time(void);

#define NK_TIME_COUNTS_PER_USECOND ...
#define NK_TIME_COUNTS_PER_SECOND ...
```

Retrieve current time in ticks.

## Microsecond timer

### nk_udelay()

```c
void nk_udelay(nk_time_t usecs);
```

Delay for a specified number of microseconds.  This is a busy-loop delay
that repeatedly calls nk_get_time(), so the CPU is not sleeping.

## Console UART

This is an interrupt driver UART device driver.

Parameters:

Set UART input buffer size.

```c
#define NK_UART_RXBUF_SIZE 128
```

### nk_init_uart()

```c
void nk_init_uart();
```

Initialize UART.

### nk_putc()

```c
void nk_putc(char c);
```

Send a character to the console.

### nk_puts()

```c
void nk_puts(const char *s);
````

Send a NUL terminated string to the console.

### nk_uart_write()

```c
void nk_uart_write(const char *s, int len);
```

Write data to the UART.

### nk_set_uart_mode(), nk_get_uart_mode()

```c
int nk_set_uart_mode(int mode);
int nk_get_uart_mode();
```

Set UART mode: 0 for text, or 1 for binary.  Text mode is the default.  In
text mode, carriage returns are automatically inserted before newlines on
output.  In binary mode, data is sent as-is.

nk_set_uart_mode returns the original mode.

### nk_getc()

```c
int nk_getc();
```

Get a character from the console keyboard.  Returns 0 - 255 with the
character, or -1 if no characters are available now.

### nk_kbhit()

```c
int nk_kbhit();
```

Return true if there is pending UART input.

### nk_uart_read()

```c
int nk_uart_read(char *s, int len, nk_time_t timeout);
```

Read data from UART.  Returns number of bytes read.  Stops reading when
buffer is full or timeout microseconds have past.  The timeout uses a
busy-loop timer based on nk_get_time().

### nk_set_uart_callback()

```c
void nk_set_uart_callback(int tid, void (*f)(void *data), void *data);
```

Register a callback function with the UART module.  When input data becomes
available, this callback is scheduled.  If data is available at the time
when nk_set_uart_callback is called, the callback is immediately scheduled. 
Scheduled means that nk_sched() is called with a dly parameter of zero.

Note that the callback is scheduled only once.  Once the callback has been
called, it must call nk_set_uart_callback again if it wants to wait for more
input.

## Flash Memory

These functions provide access to the primary NOR-flash memory of the
system.  They are used by firmware update functions such as Zmodem.

### nk_init_flash()

```c
int nk_init_flash();
```

Initialize flash memory driver.

Return 0 for success, -1 for error.

### NK_MCUFLASH_ERASE_SIZE

```c
#define NK_MCUFLASH_ERASE_SIZE ...
```

This is the minimum size that nk_flash_erase can erase.

### nk_mcuflash_erase()

```c
int nk_mcuflash_erase(const void *info, uint32_t address, uint32_t byte_count);
```

Erase 1 or more sectors.  Address and byte_count should both be multiples of
the sector size.

'info' is unused, but exists to make this function compatible with
nk_spiflash_erase.

Return 0 for success, -1 for error.

### nk_mcuflash_write()

```c
int nk_mcuflash_write(const void *info, uint32_t address, const uint8_t *data, uint32_t byte_count);
```

Write to flash.  This handles any number for byte_count- it will break up
the write into multiple page writes as necessary.

'info' is unused, but exists to make this function compatible with
nk_spiflash_write.

Return 0 for success, -1 for error.

### nk_mcuflash_read()

```c
int nk_mcuflash_read(const void *info, uint32_t address, uint8_t *data, uint32_t byte_count);
```

Read from flash.  address and byte_count can be any values- the flash memory
automatically crosses page boundaries.

'info' is unused, but exists to make this function compatible with
nk_spiflash_write.

Return 0 for success, -1 for error.
