// Copyright 2021 NK Labs, LLC

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

// nkprintf options

// Include files needed for these options
#include "nkuart.h"
#include "nkarch.h"

// Console output function
#define NKPRINTF_PUTC(c) nk_putc(c)

// Macro to lock console during Printf() if desired
//#define NKPRINTF_LOCK unsigned long irq_flag; nk_irq_lock(&console_lock, irq_flag);
#define NKPRINTF_LOCK

// Macro to unlock console
//#define NKPRINTF_UNLOCK nk_irq_unlock(&console_lock, irq_flag);
#define NKPRINTF_UNLOCK

// Disable floating point support
// #define NKPRINTF_NOFLOAT
