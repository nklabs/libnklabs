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
#include <inttypes.h> // For PRI macros

// These are missing on AVR
#ifndef PRIu64
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRId64 "lld"
#endif

#include "nkoutfile.h"
#include "nkinfile.h"
#include "nkmacros.h"
#include "nkarch.h" // For NK_FLASH

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

#ifdef NK_PSTR
// Adorn format strings with PSTR() for AVR
// Note that format string type checking is disabled for this case

int _nk_vprintf(nkoutfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_vprintf(a, b, ...) _nk_vprintf((a), PSTR(b), ##__VA_ARGS__)

int _nk_fprintf(nkoutfile_t *f, const NK_FLASH char *fmt, ...);// __attribute__((__format__ (__printf__, 2, 3)));
#define nk_fprintf(a, b, ...) _nk_fprintf((a), PSTR(b), ##__VA_ARGS__)

// print to console
// Output goes to NKPRINTF_PUTC from nkprintf_config.h

void _nk_printf(const NK_FLASH char *fmt,...);// __attribute__((__format__ (__printf__, 1, 2)));
#define nk_printf(a, ...) _nk_printf(PSTR(a), ##__VA_ARGS__)

// print to string, string will always be NUL terminated
// Returns -1 if string overflowed, 0 otherwise

int _nk_snprintf(char *dest, size_t len, const NK_FLASH char *fmt,...);// __attribute__((__format__ (__printf__, 3, 4)));
#define nk_snprintf(a, b, c, ...) _nk_snprintf((a), (b), PSTR(c), ##__VA_ARGS__)

// print to field, field will be space filled
// Returns -1 if string overflowed, 0 otherwise

int _nk_sfprintf(char *dest, size_t len, const NK_FLASH char *fmt,...);// __attribute__((__format__ (__printf__, 3, 4)));
#define nk_sfprintf(a, b, c, ...) _nk_sfprintf((a), (b), PSTR(c), ##__VA_ARGS__)

#else

int _nk_vprintf(nkoutfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_vprintf(a, b, ...) _nk_vprintf((a), (b), ##__VA_ARGS__)

int _nk_fprintf(nkoutfile_t *f, const NK_FLASH char *fmt, ...) __attribute__((__format__ (__printf__, 2, 3)));
#define nk_fprintf(a, b, ...) _nk_fprintf((a), (b), ##__VA_ARGS__)

// print to console
// Output goes to NKPRINTF_PUTC from nkprintf_config.h

void _nk_printf(const NK_FLASH char *fmt,...) __attribute__((__format__ (__printf__, 1, 2)));
#define nk_printf(a, ...) _nk_printf((a), ##__VA_ARGS__)

// print to string, string will always be NUL terminated
// Returns -1 if string overflowed, 0 otherwise

int _nk_snprintf(char *dest, size_t len, const NK_FLASH char *fmt,...) __attribute__((__format__ (__printf__, 3, 4)));
#define nk_snprintf(a, b, c, ...) _nk_snprintf((a), (b), (c), ##__VA_ARGS__)

// print to field, field will be space filled
// Returns -1 if string overflowed, 0 otherwise

int _nk_sfprintf(char *dest, size_t len, const NK_FLASH char *fmt,...) __attribute__((__format__ (__printf__, 3, 4)));
#define nk_sfprintf(a, b, c, ...) _nk_sfprintf((a), (b), (c), ##__VA_ARGS__)

#endif

extern nkoutfile_t __nkstdout;
extern nkoutfile_t __nkstdnull;

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
