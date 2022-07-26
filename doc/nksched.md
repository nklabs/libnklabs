# nksched: Minimal work queue scheduler

## Files

[nksched.h](../inc/nksched.h), [nksched.c](../src/nksched.c),
[nksched_config.h](../config/nksched_config.h)

nksched_config.h parameters:

Set work queue size: this is the maximum number of pending functions which
can be waiting in the work queue.

```c
#define WORK_QUEUE_SIZE 20
```

Description:

This is the basis for a simple cooperative multi-tasking system requiring
very little memory and no other OS.  It is suitable for small
microcontrollers but works for larger systems as well.  It can be used in
the kernel or in the user space of multi-tasking operating systems such as
Linux or RTEMS.

The heap (malloc) is not used.

There is no pre-emption and only one stack is required.

The size of the queue (the maximum number of pending functions) can be
set with a #define.

There is a queue of functions waiting to be called.  When a function
previously called by the scheduler returns to the main loop, the next
waiting function is called.  If there are no functions waiting, then the
microcontroller sleeps.

Functions can be scheduled to execute now, or at some time in the future. 
"Now" means the function is executed on the next return to the main loop.  "In
the future" means that the system sleeps for the time needed before the next
scheduled function in the queue, wakes up, and calls that function.

If multiple functions are scheduled to execute at the same time, then
they are serviced in FIFO order (first one submitted goes first).

Waiting functions can be canceled or rescheduled.

When a function is scheduled, a data parameter (type "void *") is stored
along with the function address, and is passed as the first argument
when the function is called.

Since there is no preemption, functions do not have to worry about locking
shared resources (as long as sharing does not cross multiple returns to the
scheduler).  However, functions do have to worry about contention from
interrupt handlers.

### nk_init_sched()

```c
void nk_init_sched(void);
```

Initialize nksched module.

This calls nk_init_sched_timer().

### nk_sched_loop()

```c
void nk_sched_loop(void);
```

Enter scheduler main loop.  This calls scheduled functions at the proper
times.  It never returns.


### nk_alloc_tid()

```c
int nk_alloc_tid();
```

Allocate a task ID number.  This is a number used to identify a function
waiting in the work queue.  There can only be a single function with a given
task ID number waiting in the queue.  The same function can appear in the
work queue multiple times, but each instance must have a unique task ID
number.

Task IDs are just numbers, there is no other data is associated with them
when nk_alloc_tid is called.  nk_alloc_tid just increments a counter to
provide the next available number.

There is no provision to free task ID numbers.

Zero is never returned.  This is to distinguish uninitialized task IDs from valid
task IDs.

### nk_get_tid()

```c
int nk_get_tid();
```

Get task ID number of currently executing function.

### nk_sched()

```c
int nk_sched(int tid, void (*f)(void *dat), void *dat, uint32_t dly, const char *note);
```

Schedule a function 'f' for future execution.  The main loop will call it a
minimum of 'dly' ms in the future.  A delay of 0 means it will be called
when we next return to the main loop.  The 'note' is text printed in the
output of the "work" CLI command to help identify the pending task.

'dat' is retained in the work queue and is passed as the first argument to
'f'.

nk_sched is safe to call from interrupt handlers

If a function for the specified task id (tid) already exists in the work
queue, it is replaced with this new function.

Returns 1 if function was enqueued and there was no previous function in
the queue with the same the tid.

Returns 0 if there was a previous function in the queue with the same tid
that was replaced with this new function.

### nk_resched()

```c
int nk_resched(int tid, void (*f)(void *dat), void *dat, uint32_t dly);
```

Reschedule an already scheduled function.  Unlike nk_sched, this won't
schedule the function unless it already exists.

nk_resched is safe to call from interrupt handlers

Returns 1 if there was a previous function in the queue with the same tid
that was replaced with this new function.

Returns 0 if there is no function with the specified tid waiting in the
queue (it had already executed or was never scheduled).

### nk_unsched()

```c
int nk_unsched(int tid);
```

Unschedule a function

nk_unsched is safe to call from interrupt handlers

Returns 1 if function waiting in the queue for the specified tid was
removed.

Returns 0 if there is no such function (it had already executed or was never
scheduled).

### nk_set_sched_sleep_mode(), nk_get_sched_sleep_mode()

```c
void nk_set_sched_sleep_mode(int deepness);
int nk_get_sched_sleep_mode();
```

Control sleep behavior

If 'deepness' is true, microcontroller is put into deep sleep when there is
no work.  This saves power, but may incur a longer wake-up time.

Otherwise sleeping is light and wake-up time is minimized.

### nksched example

This example shows how to blink two LEDs at different rates using nksched:

```c
struct per_led {
	int tid, led_no, blink_delay, state;
} led1, led2;


void blink(void *data)
{
	struct per_led *led = (struct per_led *)data; // Get per_led structure
	led->state = !led->state; // Invert LED state
	gpio_set(led->led_no, led->state); // Drive GPIO for LED
	nk_sched(led->tid, blink, data, led->blink_delay, "Blinker"); // Come back later
}

int main()
{
	nk_init_sched(); // Initialize scheduler

	// Set up LED 1
	led1.tid = nk_alloc_tid();
	led1.led_no = 1;
	led1.blink_delay = 125;

	// Set up LED 2
	led2.tid = nk_alloc_tid();
	led2.led_no = 2;
	led2.blink_delay = 375;

	// Call first blink directly, this will schedule the next one
	blink(&led1);
	blink(&led2);

	nk_sched_loop(); // Execute waiting tasks
}
```
