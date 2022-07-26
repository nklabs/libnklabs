// Simple scheduler

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

#ifndef _Inksched
#define _Inksched

#include "nksched_config.h"
#include "nkarch.h"
#include "nkmacros.h"

// Submit a callback function to the work queue.  The main loop will call it
// a minimum of 'delay' ms in the future.  A delay of 0 means it will be
// called when we next return to the main loop.
//
// This is safe to call from interrupt handlers
//
// If task already exists, it is rescheduled with the new delay.
//
// Returns 1 if new task was submitted
// Returns 0 if existing task was rescheduled


int _nk_sched(int tid, void (*func)(void *data), void *data, uint32_t delay, const char *name, const char *by, const char *comment);

#define nk_sched(tid, func,data,delay,comment) _nk_sched(tid, func, data, delay, #func, __FILE__ ":" nk_tostring(__LINE__), comment)

// Reschedule an already submitted callback function.  Unlike submit, this does not
// submit the callback if it does not already exist.
//
// This is safe to call from interrupt handlers
//
// Returns 1 if task rescheduled
// Returns 0 if there is no such task (it had already executed or was never submitted)

int nk_resched(int tid, void (*func)(void *data), void *data, uint32_t delay);

// Cancel a callback function
//
// This is safe to call from interrupt handlers
//
// Returns 1 if task was canceled
// Returns 0 if there is no such task (it had already executed or was never submitted)

int nk_unsched(int tid);

int nk_check(int tid);

// Main loop

void nk_sched_loop(void);

// Initialize scheduler

void nk_init_sched(void);

int nk_get_tid();
int nk_alloc_tid();
int nk_get_sched_sleep_mode();
int nk_set_sched_sleep_mode(int deepness);

extern spinlock_t sched_lock;

#endif
