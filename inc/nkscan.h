// Parser: this is just like sscanf but allows for composition.
// It advances the string pointer over the parsed text if it succeeds.
// Returns 0 if parse fails, ptr not advanced
// Returns 1 if parse succeeds, ptr advanced

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

#ifndef _nkscan
#define _nkscan

#include <stdint.h> // For uint64_t
#include <stdlib.h> // For ssize_t
#include <stdarg.h>

//#include <inttypes.h> // For PRI macros
#include "nkprintf.h" // For PRI macros

#include "nkarch.h"

#include "nkinfile.h"

#include "nkscan_config.h"

// nk_fscan: string parser, somewhat like sscanf but better
//
// Each character in the format string must match the next character from
// the input string, except:
//
//  Space matches zero or more spaces or tabs
//
//  %%  Matches %
//
//  %hhd   Matches a decimal number, stores result into a char.  A (char *) should be provided.
//  %hd    Matches a decimal number, sotres result into a short.  A (short *) should be provided.
//  %d     Matches a decimal number, stores result into an int.  An (int *) should be provided.
//  %ld    Matches a decimal number, stores result into a long.  A (long *) should be provided.
//  %lld   Matches a decimal number, stores result into a long long.  A (long long *) should be provided.
//  %zd    Matches a decimal number, stores result into a size_t.  A (size_t *) should be provided.
//  %td    Matches a decimal number, stores result into a ptrdiff_t.  A (ptrdiff_t *) should be provided.
//
//  %hhx etc.   Same as above, but hexadecimal number into unsigned
//
//  %hhu etc.   Same as above, but decimal number into unsigned
//
//  %f     Matches a floating point number.  A (double *) should be provided.
//
//  %p     Matches a pointer in the form of a hexadecimal number, stores result into a void *.  A (void **) should be provided.
//
//  %i     Save identifier in a string buffer.  The identifier is composed of [a-zA-Z_][a-zA-Z_0-9]*
//         Both the address of the string (char *) and the length of the string (size_t) should be provided.
//
//         If the identifier begins with ", then assume it's a C string.  All characters up to the following " are included in the
//         identifier, and C string escape sequences are available from within the string.
//
//  %w     Save word in a string buffer.  The word is composed of non-whitespace chars followed by whitespace or end of string
//         Both the address of the string (char *) and the length of the string (size_t) should be provided.
//
//         If the word begins with ", then assume it's a C string.  All characters up to the following " are included in the
//         word, and C string escape sequences are available from within the string.
//
//  %v     Match an nkdbase serialized database.  (struct type *ty) and (void *ram) should be provided.  'ty' is schema for database.
//         'ram' is where parsed database should be placed.
//
//  %e     Matches end of string (end of string is required at this point, and no further characters should be in the format string)
//
//  Parse succeeds (returns 1) only if the entire format string is matched.
//
//  A field width may be specified after any %, like %2d.  This will limit the parsing to the indicated size.  WIth %2d, only the first two
//  digits will be taken.  For example, you could parse a date such as 20181231 with "%4d%2d%2d", &year, &month, &day


#ifdef NK_PSTR

int _nk_vscan(nkinfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_vscan(a, b, ...) _nk_vscan((a), PSTR(b), ##__VA_ARGS__)

int _nk_fscan(nkinfile_t *f, const NK_FLASH char *fmt, ...);
#define nk_fscan(a, b, ...) _nk_fscan((a), PSTR(b), ##__VA_ARGS__)

int _nk_viscan(nkinfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_viscan(a, b, ...) _nk_viscan((a), PSTR(b), ##__VA_ARGS__)

int _nk_fiscan(nkinfile_t *f, const NK_FLASH char *fmt, ...);
#define nk_fiscan(a, b, ...) _nk_fiscan((a), PSTR(b), ##__VA_ARGS__)

#else

int _nk_vscan(nkinfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_vscan(a, b, ...) _nk_vscan((a), (b), ##__VA_ARGS__)

int _nk_fscan(nkinfile_t *f, const NK_FLASH char *fmt, ...);
#define nk_fscan(a, b, ...) _nk_fscan((a), (b), ##__VA_ARGS__)

int _nk_viscan(nkinfile_t *f, const NK_FLASH char *fmt, va_list ap);
#define nk_viscan(a, b, ...) _nk_viscan((a), (b), ##__VA_ARGS__)

int _nk_fiscan(nkinfile_t *f, const NK_FLASH char *fmt, ...);
#define nk_fiscan(a, b, ...) _nk_fiscan((a), (b), ##__VA_ARGS__)

#endif

int nk_fscan_ws(nkinfile_t *f);
int nk_fscan_escape(nkinfile_t *f);
int nk_fscan_word(nkinfile_t *f, char *buf, int width, size_t buf_size);
int nk_fscan_ident(nkinfile_t *f, char *buf, int width, size_t buf_size);
int nk_fiscan_word(nkinfile_t *f, char *buf, int width, size_t buf_size);
int nk_fiscan_ident(nkinfile_t *f, char *buf, int width, size_t buf_size);
int nk_fscan_hex(nkinfile_t *f, uint64_t *val, int width);
int nk_fscan_dec(nkinfile_t *f, uint64_t *val, int width);
int nk_fscan_double(nkinfile_t *f, double *val);

#endif
