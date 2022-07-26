// nkprintf options

#include <stdio.h>

// Console output function
#define NKPRINTF_PUTC(c) putchar(c)

// Macro to lock console during Printf() if desired
//#define NKPRINTF_LOCK unsigned long irq_flag; nk_irq_lock(&console_lock, irq_flag);

// Macro to unlock console
//#define NKPRINTF_UNLOCK nk_irq_unlock(&console_lock, irq_flag);

// Disable floating point support
// #define NKPRINTF_NOFLOAT
