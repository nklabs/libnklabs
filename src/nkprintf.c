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

#include <stdarg.h>
#include <string.h>
#include <stddef.h>
#include "nkprintf.h"

#ifndef NKPRINTF_NOFLOAT
#include "nkprintf_fp.h"
#endif

int nk_findent(nkoutfile_t *f, int ind)
{
    int status = 0;
    while (ind--) {
        status |= nk_fputc(f, ' ');
    }
    return status;
}

int nk_indent(int ind)
{
    return nk_findent(nkstdout, ind);
}

int nk_fbyte_hex_dump(nkoutfile_t *f, unsigned char *buf, unsigned long base, unsigned long offset, unsigned long count)
{
	int status = 0;
	unsigned int y, x;
	unsigned int start = (offset & 15);
	offset &= ~15UL;
	for (y = 0; count; y += 16) {
		unsigned int st = start;
		unsigned long cn = count;
		status |= nk_fprintf(f, "%4.4lx:", base + offset + y);
		for (x = 0; x != 16; ++x) {
			if (x == 8)
				status |= nk_fprintf(f, " ");
			if (st || cn == 0) {
				status |= nk_fprintf(f, "   ");
				if (st)
					--st;
			} else {
				status |= nk_fprintf(f, " %2.2x", buf[offset + y + x]);
				--cn;
			}
		}
		status |= nk_fprintf(f, "  ");
		for (x = 0; x != 16; ++x) {
			int c = buf[offset + y + x];
			if (c < 32)
				c = '.';
			if (c >= 127 && c <= 255)
				c = '.';
			if (start || count == 0) {
				status |= nk_fprintf(f, " ");
				if (start)
					--start;
			} else {
				status |= nk_fprintf(f, "%c", c);
				--count;
			}
		}
		status |= nk_fprintf(f, "\n");
	}
	return status;
}

int nk_byte_hex_dump(unsigned char *buf, unsigned long base, unsigned long offset, unsigned long count)
{
	return nk_fbyte_hex_dump(nkstdout, buf, base, offset, count);
}

// Small printf()

int _nk_vprintf(nkoutfile_t *f, const NK_FLASH char *fmt, va_list ap)
{
	int status = 0;

	while (*fmt) {
		if (*fmt != '%') {
			status |= nk_fputc(f, *fmt++);
		} else {
			unsigned long long val;
			const char *s;
			size_t n;
			size_t len; // Length of number in buf
			int hex = 0; // Set for hex, clear for decimal number
			int upr = 0; // Set for uppercase hex
			int minus = 0; // Left justified flag
			int plus = 0; // Prefix '+' flag
			int space = 0; // Prefix with space for positive numbers flag
			int pound = 0; // Prefix hex with 0x unless zero

			int dot __attribute__((unused)) = 0; // Dot was given
			size_t width = 0; // Field width
			size_t prec = 0; // Precision

			int size = 0; // Argument size: 0 = int, 1 = long, 2 = long long, -1 = short, -2 = char
			int dbl = 0; // Double precision

			int neg = 0; // Number is negative
			size_t prefix = 0; // Number of prefix characters
			int fill __attribute__((unused)) = 0; // Insert underscores or commas into numbers
			char buf[24];
			char pad = ' ';
			char c;
			// Flags
			for (;;) {
				switch (*++fmt) {
					case '-': minus = 1; continue;
					case '+': plus = 1; continue;
					case ' ': space = 1; continue;
					case '#': pound = 1; continue;
					case '0': pad = '0'; continue;
					case '_': fill = '_'; continue;
					case ',': fill = ','; continue;
				}
				break;
			}
			// Width
			if (*fmt == '*') {
				++fmt;
				width = (size_t)(int)va_arg(ap, int);
			} else {
				while (*fmt >= '0' && *fmt <= '9') {
					width = width * 10 + *fmt++ - '0';
				}
			}
			if (*fmt == '.') {
				dot = 1;
				++fmt;
				// Precision
				if (*fmt == '*') {
					++fmt;
					prec = (size_t)(int)va_arg(ap, int);
				} else {
					while (*fmt >= '0' && *fmt <= '9') {
						prec = prec * 10 + *fmt++ - '0';
					}
				}
			}
			// Size
			--fmt;
			for (;;) {
				switch (*++fmt) {
					case 'z': size = 3; continue; // size_t
					case 't': size = 4; continue; // ptrdiff_t
					case 'l': ++size; continue;
					case 'h': --size; continue;
					case 'L': ++dbl; continue;
				}
				break;
			}
			// This character must be the specifier
			switch (c = *fmt++) {
#ifndef NKPRINTF_NOFLOAT
				case 'A': case 'a':
				case 'G': case 'F': case 'E':
				case 'g': case 'f': case 'e': { // Floating point
					double fp = (double)va_arg(ap, double);
					int formatted_len = sizeof(buf); // Buffer size on input, printed length on output (or -1 for error)
					int fp_flags = 0;
					if (plus) fp_flags |= FLplus;
					if (pound) fp_flags |= FLhash;
					if (space) fp_flags |= FLspc;
					char *pfx = __floatfmt(
						c,
						fp_flags,
						prec,
						&fp,
						buf,
						&formatted_len,
						width
					);
					if (formatted_len < 0) {
						status |= nk_fputc(f, '!');
					} else {
						int i;
						for (i = 0; pfx[i]; ++i)
							status |= nk_fputc(f, pfx[i]);
						for (i = 0; i < formatted_len; ++i)
							status |= nk_fputc(f, buf[i]);
					}
					break;
				}
#endif
				case 'c': { // Character
					buf[0] = (char)va_arg(ap, int);
					s = buf;
					len = 1;
					goto emits;
					rev: // Emit number from buf
					// At least one digit
					if (len == 0) {
						buf[len++] = '0';
					} else if (pound && hex) {
						buf[len++] = 'x'; ++prefix;
						buf[len++] = '0'; ++prefix;
					}
					if (neg) {
						buf[len++] = '-'; ++prefix;
					} else {
						if (plus) {
							buf[len++] = '+'; ++prefix;
						} else if (space) {
							buf[len++] = ' '; ++prefix;
						}
					}
					// Reverse digits
					for (n = 0; n < len/2; ++n) {
						char t = buf[n];
						buf[n] = buf[len - 1 - n];
						buf[len - 1 - n] = t;
					}
					s = buf;
					emits: // Emit string in s
					if (minus) { // Left justify
						/* Emit prefix */
						while (prefix--) {
							status |= nk_fputc(f, *s++);
							if (width)
								--width;
							--len;
						}
						/* Extend to requested precision */
						while (prec > len) {
							status |= nk_fputc(f, '0');
							if (width)
								--width;
							--prec;
						}
						/* Emit number */
						while (len--) {
							status |= nk_fputc(f, *s++);
							if (width)
								--width;
						}
						/* Padding: always space for left justified */
						while (width) {
							status |= nk_fputc(f, ' ');
							--width;
						}
					} else { // Right justify
						size_t full_len = len;
						if (prec > len - prefix)
							full_len = prec + prefix;
						if (pad == ' ') { // Space padding comes before the '-'
							/* Padding */
							while (width > full_len) {
								status |= nk_fputc(f, pad);
								--width;
							}
							/* Emit prefix */
							while (prefix--) {
								status |= nk_fputc(f, *s++);
								--len;
							}
						} else { // Zero padding comes after the '-'
							/* Emit prefix */
							while (prefix--) {
								status |= nk_fputc(f, *s++);
								if (width)
									--width;
								--full_len;
								--len;
							}
							/* Padding */
							while (width > full_len) {
								status |= nk_fputc(f, pad);
								--width;
							}
						}
						/* Extend to requested precision */
						while (prec > len) {
							status |= nk_fputc(f, '0');
							--prec;
						}
						/* Emit number */
						while (len--) {
							status |= nk_fputc(f, *s++);
						}
					}
					break;
				} case 'i': case 'd': { // Signed decimal
					if (size == 1)
						val = (unsigned long long)(long long)va_arg(ap, long);
					else if (size == 2)
						val = (unsigned long long)(long long)va_arg(ap, long long);
					else if (size == 3)
						val = (unsigned long long)(long long)va_arg(ap, size_t);
					else if (size == 4)
						val = (unsigned long long)(long long)va_arg(ap, ptrdiff_t);
					else
						val = (unsigned long long)(long long)va_arg(ap, int);
					if ((long long)val < 0) {
						neg = 1;
						val = -val;
					}
					dec:
					// Convert decimal to string
					for (len = 0; val; val /= 10, ++len) {
						buf[len] = (char)('0' + (val % 10));
					}
					s = buf;
					goto rev;
					break;
				} case 'u': case 'x': case 'X': case 'p': { // Unsigned decimal or hex
					if (c == 'p') // Pointer
						val = (uintptr_t)va_arg(ap, void *);
					else if (size == 1)
						val = va_arg(ap, unsigned long);
					else if (size == 2)
						val = va_arg(ap, unsigned long long);
					else if (size == 3)
						val = va_arg(ap, size_t);
					else if (size == 4)
						val = (unsigned long long)va_arg(ap, ptrdiff_t);
					else
						val = va_arg(ap, unsigned int);
					if (c == 'x' || c == 'p') {
						hex = 1;
					} else if (c == 'X') {
						hex = 1;
						upr = 1;
					}
					if (!hex)
						goto dec;
					// Convert hex to string
					for (len = 0; val; val >>= 4, ++len) {
						int v = (val & 0xF);
						if (v < 10)
							buf[len] = (char)('0' + v);
						else if (upr)
							buf[len] = (char)('A' + v - 10);
						else
							buf[len] = (char)('a' + v - 10);
					}
					goto rev;
					break;
				} case 's': { // String
					s = va_arg(ap, char *);
					len = strlen(s);
					goto emits;
					break;
#ifdef NK_PSTR
				} case 'S': { // AVR String located in flash memory
					const NK_FLASH char *S = va_arg(ap, char *);
					len = strlen_P(S);
					if (minus) { // Left justify
						/* Emit string */
						while (len--) {
							status |= nk_fputc(f, *S++);
							if (width)
								--width;
						}
						/* Padding: always space for left justified */
						while (width) {
							status |= nk_fputc(f, ' ');
							--width;
						}
					} else { // Right justify
						/* Padding */
						while (width > len) {
							status |= nk_fputc(f, pad);
							--width;
						}
						/* Emit string */
						while (len--) {
							status |= nk_fputc(f, *S++);
						}
					}
					break;
#endif
				} case '_': { // Nothing: use with %*_ to print N spaces
					s = "";
					len = 0;
					goto emits;
					break;
				} case 0: { // End of format string?
					--fmt;
					break;
				} default: { // Unknown or '%'
					status |= nk_fputc(f, c);
					break;
				}
			}
		}
	}

	return status;
}

// Print to string, NUL always written

int _nk_snprintf(char *dest, size_t len, const NK_FLASH char *fmt, ...)
{
	nkoutfile_t f;
	int status;
	va_list ap;

	va_start (ap, fmt);

	nkoutfile_open_mem(&f, dest, len - 1);

	status = _nk_vprintf(&f, fmt, ap);

	*f.ptr = 0;

	va_end(ap);
	return status;
}

// Print to string, balance of string is space filled.  No NUL!

int _nk_sfprintf(char *dest, size_t len, const NK_FLASH char *fmt, ...)
{
	nkoutfile_t f;
	int status;
	va_list ap;

	va_start (ap, fmt);

	nkoutfile_open_mem(&f, dest, len);

	status = _nk_vprintf(&f, fmt, ap);

	while (f.ptr != f.end) {
		*f.ptr++ = ' ';
	}

	va_end(ap);
	return status;
}

// nkstdout set up for console with no buffering

static int outc(void *ptr, size_t offset, const unsigned char *buf, size_t len)
{
	(void)ptr;
	(void)offset;
	while (len--)
		NKPRINTF_PUTC(*buf++);
	return 0;
}

static nkoutfile_t __nkstdout =
{
	.ptr = 0,
	.start = 0,
	.end = 0,
	.size = 1,
	.block_write_ptr = 0,
	.block_write = outc,
	.granularity = 1
};

static nkoutfile_t __nkstdnull =
{
 	.ptr = 0,
	.start = 0,
	.end = 0,
	.size = 0,
	.block_write_ptr = 0,
	.block_write = 0,
	.granularity = 1
};

nkoutfile_t *nkstdout = &__nkstdout;
nkoutfile_t *nkstderr = &__nkstdout;
nkoutfile_t *nkstdnull = &__nkstdnull;

// Print to console

void _nk_printf(const NK_FLASH char *fmt,...)
{
	va_list ap;
	va_start (ap, fmt);

#ifdef NKPRINTF_LOCK
        NKPRINTF_LOCK
#endif

	_nk_vprintf(nkstdout, fmt, ap);

#ifdef NKPRINTF_UNLOCK
	NKPRINTF_UNLOCK
#endif

	va_end(ap);
}

// Print to file

int _nk_fprintf(nkoutfile_t *f, const NK_FLASH char *fmt,...)
{
	int status;
	va_list ap;
	va_start (ap, fmt);

	status = _nk_vprintf(f, fmt, ap);

	va_end(ap);

	return status;
}
