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

#ifndef _NKUART_H
#define _NKUART_H

#include "nkuart_config.h"

#include "nkarch.h"

// Get character from input buffer or return -1 if none available
int nk_getc();

// True if any input available
int nk_kbhit();

// Read data from UART
int nk_uart_read(char *s, int len, uint32_t timeout);

// Set task to trigger when characters are available
void nk_set_uart_callback(int tid, void (*func)(void *data), void *data);

// Put character, convert \n to \r\n
void nk_putc(char ch);

// Write string
void nk_puts(const char *s);

// Write data
void nk_uart_write(const char *s, int len);

extern nk_spinlock_t console_lock;

int nk_set_uart_mode(int new_mode);
int nk_get_uart_mode();

// Initialize UART
void nk_init_uart();

int nk_get_uart_mode();
int nk_set_uart_moed(int new_mode);

#endif   /* _NKUART_H */
