// Scheduler / work-queue

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


#include "nkmacros.h"
#include "nkcli.h"
#include "nksched.h"


nk_spinlock_t sched_lock = SPIN_LOCK_UNLOCKED;

static int current_tid;

int nk_get_tid()
{
	return current_tid;
}

static int next_tid;

int nk_alloc_tid()
{
	return ++next_tid;
}

// Work queue items.  First item is base of queue, rest are free items to use.

static struct item {
	struct item *next, *prev;
	int tid;
	void (*func)(void *data);
	void *data;
	nk_time_t when; // Time in timer ticks
	const NK_FLASH char *comment;
	const NK_FLASH char *name;
	const NK_FLASH char *by;
} queue[NK_WORK_QUEUE_SIZE];

static struct item *freelist;

static struct item *alloc_item()
{
	struct item *i = freelist;
	if (i)
		freelist = i->next;
	return i;
}

static void free_item(struct item *i)
{
	i->next = freelist;
	freelist = i;
}

static struct item *deque(struct item *i)
{
	i->prev->next = i->next;
	i->next->prev = i->prev;
	return i;
}

static void enque(struct item *q, struct item *i)
{
	i->next = q;
	i->prev = q->prev;
	q->prev->next = i;
	q->prev = i;
}

static int queue_not_empty()
{
	return queue != queue->next;
}

// delay in milliseconds
int _nk_sched(int tid, void (*func)(void *data), void *data, uint32_t delay, const NK_FLASH char *name, const NK_FLASH char *by, const NK_FLASH char *comment)
{
	struct item *item;
	nk_irq_flag_t irq_flag;
	int rtn = 0;

	// Convert delay in milliseconds to ticks
	delay = nk_convert_delay(delay);

	irq_flag = nk_irq_lock(&sched_lock);

	// Maybe this function is already queued.. if so reschedule
	for (item = queue->next; item != queue; item = item->next)
		if (item->tid == tid)
			break;
	if (item == queue) {
		item = alloc_item();
		rtn = 1;
	} else {
		deque(item);
		rtn = 0;
	}
	if (item) {
		struct item *q;
		item->tid = tid;
		item->func = func;
		item->data = data;
		item->when = nk_get_time() + delay;
		item->name = name;
		item->by = by;
		item->comment = comment;
		// Find first item ahead of us in time, or end of queue
		for (q = queue->next; q != queue; q = q->next)
			if ((int32_t)(q->when - item->when) > 0)
				break;
		// Insert new item before this one
		enque(q, item);
	} else {
		// Not enough work queue entries!
		nk_error_message("Too many work queue entries, task could not be submitted!\n");
	}
	nk_irq_unlock(&sched_lock, irq_flag);
	return rtn;
}

int nk_resched(int tid, void (*func)(void *data), void *data, uint32_t delay)
{
	struct item *item;
	nk_irq_flag_t irq_flag;
	int rtn = 0;

	// Convert delay in milliseconds to ticks
	delay = nk_convert_delay(delay);

	irq_flag = nk_irq_lock(&sched_lock);

	// Look for tiem
	for (item = queue->next; item != queue; item = item->next)
		if (item->tid == tid)
			break;
	if (item == queue) { // Not found
		rtn = 0;
	} else { // Found
		struct item *q;
		deque(item);
		item->when = nk_get_time() + delay;
		item->func = func;
		item->data = data;
		// Find first item ahead of us in time, or end of queue
		for (q = queue->next; q != queue; q = q->next)
			if ((int32_t)(q->when - item->when) > 0)
				break;
		// Insert new item before this one
		enque(q, item);
		rtn = 1;
	}
	nk_irq_unlock(&sched_lock, irq_flag);
	return rtn;
}

int nk_unsched(int tid)
{
	struct item *item;
	nk_irq_flag_t irq_flag;
	int rtn = 0;

	irq_flag = nk_irq_lock(&sched_lock);

	// Look for item
	for (item = queue->next; item != queue; item = item->next)
		if (item->tid == tid)
			break;
	if (item == queue) { // Item not found
		rtn = 0;
	} else { // Found
		deque(item);
		free_item(item);
		rtn = 1;
	}
	nk_irq_unlock(&sched_lock, irq_flag);
	return rtn;
}

// Return true if job exists in queue

int nk_check(int tid)
{
	struct item *item;
	nk_irq_flag_t irq_flag;
	int rtn = 0;

	irq_flag = nk_irq_lock(&sched_lock);

	// Look for item
	for (item = queue->next; item != queue; item = item->next)
		if (item->tid == tid)
			break;
	if (item == queue) { // Item not found
		rtn = 0;
	} else { // Found
		rtn = 1;
	}
	nk_irq_unlock(&sched_lock, irq_flag);
	return rtn;
}

void nk_init_sched()
{
	int i;
	nk_startup_message("Work queue\n");
	queue->next = queue;
	queue->prev = queue;
	freelist = 0;
	for (i = 1; i != sizeof(queue) / sizeof(struct item); ++i) {
		free_item(&queue[i]);
	}
	nk_init_sched_timer();
}

static int cmd_work(nkinfile_t *args)
{
	nk_irq_flag_t irq_flag;
	struct item *i;
	if (nk_fscan(args, "")) {
		nk_printf("Current time = %lu\n", nk_get_time());
		nk_printf("Pending tasks:\n");
		irq_flag = nk_irq_lock(&sched_lock);
		if (queue->next != queue) {
			for (i = queue->next; i != queue; i = i->next) {
				/* Note that on ARM, address will be odd, due to thumb mode bit being set */
#ifdef NK_PSTR
				nk_printf(" \"%S\": tid=%d func=%S(0x%p) data=0x%p when=%lu [submitted by %S]\n", i->comment, i->tid, i->name, i->func, i->data, i->when, i->by);
#else
				nk_printf(" \"%s\": tid=%d func=%s(0x%p) data=0x%p when=%lu [submitted by %s]\n", i->comment, i->tid, i->name, i->func, i->data, i->when, i->by);
#endif
			}
		}
		nk_irq_unlock(&sched_lock, irq_flag);
		nk_printf("End of list.\n");
	} else {
		nk_printf("Syntax error\n");
	}
	return 0;
}

COMMAND(cmd_work,
	">work                      Show work queue\n"
	"-work                      Show work queue\n"
)

static int deepness = 0;

int nk_get_sched_sleep_mode()
{
	return deepness;
}

int nk_set_sched_sleep_mode(int new_deepness)
{
	int old = deepness;
	deepness = new_deepness;
	return old;
}

static int cmd_power(nkinfile_t *args)
{
	int new_mode;
	if (nk_fscan(args, "")) {
		nk_printf("Power mode = %d\n", nk_get_sched_sleep_mode());
	} else if (nk_fscan(args,"%d ", &new_mode)) {
		nk_set_sched_sleep_mode(new_mode);
		nk_printf("Power mode = %d\n", nk_get_sched_sleep_mode());
	} else {
		nk_printf("Syntax error\n");
	}
	return 0;
}

COMMAND(cmd_power,
	">power                     Show/control power mode\n"
	"-power                     Show power mode\n"
	"-power <n>                 Set power mode (0, 1 or 2)\n"
)

void nk_sched_loop()
{
	nk_irq_flag_t irq_flag;

	nk_startup_message("Begin main loop\n");

	for (;;) {
		irq_flag = nk_irq_lock(&sched_lock);
		// Execute pending work
		if (queue_not_empty() && (int32_t)(nk_get_time() - queue->next->when) >= 0)
		{
			void (*func)(void *data);
			void *data;
			struct item *item = deque(queue->next);
			func = item->func;
			data = item->data;
			current_tid = item->tid;
			free_item(item);
			nk_irq_unlock(&sched_lock, irq_flag);
			func(data);
			continue;
		}
		else
		{
			// Set alarm to wake up system for next event
			// There must be a wake up interrupt, even if the request is for now or in the past
			if (queue_not_empty())
			{
				nk_sched_wakeup(queue->next->when);
			}
			// Enable interrupts and sleep
			nk_irq_unlock_and_wait(&sched_lock, irq_flag, deepness);
			// If there are any pending interrupts, the system should wake up
		}
	}
}
