// Small versions of printf / sprintf

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

#ifndef _nkprintf
#define _nkprintf 1

#include <stdarg.h>

#include "nkoutfile.h"
#include "nkinfile.h"
#include "nkmacros.h"

// Config options

#include "nkprintf_config.h"

// Hex dump
// Print from buf[offset] to buf[offset+count]
// Base is added to offset for the printed address

int nk_fbyte_hex_dump(nkoutfile_t *f, unsigned char *buf, unsigned long base, unsigned long offset, unsigned long count);

int nk_byte_hex_dump(unsigned char *buf, unsigned long base, unsigned long offset, unsigned long count);

// print 'ind' spaces- used for indentation

int nk_findent(nkoutfile_t *f, int ind);

int nk_indent(int ind);

// Small version of printf for small MCUs
// Low level printf
// Returns return value of emitc

// Non-standard things:
//
//  _ prints an empty string.  This is useful for variable indentation, as in nk_printf("%*_Hello, world!\n", indent);
//
//  v prints an nkdbase in serialized format, for example:   nk_printf("%v", type, location);

int nk_vprintf(nkoutfile_t *f, const char *fmt, va_list ap);

int nk_fprintf(nkoutfile_t *f, const char *fmt, ...) __attribute__((__format__ (__printf__, 2, 3)));

// print to console
// Output goes to NKPRINTF_PUTC from nkprintf_config.h

void nk_printf(const char *fmt,...) __attribute__((__format__ (__printf__, 1, 2)));

// print to string, string will always be NUL terminated
// Returns -1 if string overflowed, 0 otherwise

int nk_snprintf(char *dest, size_t len, const char *fmt,...) __attribute__((__format__ (__printf__, 3, 4)));

// print to field, field will be space filled
// Returns -1 if string overflowed, 0 otherwise

int nk_sfprintf(char *dest, size_t len, const char *fmt,...) __attribute__((__format__ (__printf__, 3, 4)));

extern nkoutfile_t *nkstdout;
extern nkoutfile_t *nkstderr;
extern nkoutfile_t *nkstdnull;

/* debug_printf */

#define nk_debug_message(fmt, ...) nk_printf(__FILE__ ":" nk_tostring(__LINE__) ": " fmt, ##__VA_ARGS__)

#define nk_error_message(fmt, ...) nk_printf(__FILE__ ":" nk_tostring(__LINE__) ": Error: " fmt, ##__VA_ARGS__)

#define nk_startup_message(fmt, ...) nk_printf("[Initialize] " fmt, ##__VA_ARGS__)

#define nk_fatal_message(fmt, ...) do { \
	nk_printf(__FILE__ ":" nk_tostring(__LINE__) ": Fatal Error: " fmt, ##__VA_ARGS__); \
	for (;;) ; \
	} while (0);

#endif
