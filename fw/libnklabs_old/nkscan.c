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
#include <stdlib.h>
#include <string.h>
#include <stddef.h> // for ptrdiff_t
#include "nkscan.h"

#ifndef NKSCAN_NOFLOAT
#include "nkstrtod.h"
#endif

#ifndef NKSCAN_NODBASE
#include "nkdbase.h"
#endif

#include "nkstring.h"

int nk_fscan_ws(nkinfile_t *f)
{
	for (int c = nk_fpeek(f); nk_isspace(c); c = nk_fnext_fast(f));
	return 1;
}

// Parse one string escape sequence

int nk_fscan_escape(nkinfile_t *f)
{
	int c = nk_fpeek(f);
	int d;
	if (c == '\\') {
		c = nk_fnext_fast(f);
		d  = 0;
		if (c != -1)
			d = nk_fnext_fast(f);
		if (c == 'a') {
			c = 7;
		} else if (c == 'b') {
			c = 8;
		} else if (c == 'e') {
			c = 27;
		} else if (c == 'f') {
			c = 12;
		} else if (c == 'n') {
			c = 10;
		} else if (c == 'r') {
			c = 13;
		} else if (c == 't') {
			c = 9;
		} else if (c == '\\') {
			c = '\\';
		} else if (c == 'x') {
			c = 0;
			if (d >= '0' && d <= '9') {
				c = d - '0';
				d = nk_fnext_fast(f);
			} else if (d >= 'a' && d <= 'f') {
				c = d - 'a' + 10;
				d = nk_fnext_fast(f);
			} else if (d >= 'A' && d <= 'F') {
	 			c = d - 'A' + 10;
				d = nk_fnext_fast(f);
			}
			if (d >= '0' && d <= '9') {
				c = c*16 + d - '0';
				nk_fnext_fast(f);
			} else if (d >= 'a' && d <= 'f') {
				c = c*16 + d - 'a' + 10;
				nk_fnext_fast(f);
			} else if (d >= 'A' && d <= 'F') {
	 			c = c*16 + d - 'A' + 10;
				nk_fnext_fast(f);
			}
		}
	} else if (c != -1) {
		nk_fnext_fast(f);
	}
	return c;
}

static int _nk_fscan_word(int quash_case, nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	int c;
	int status = 0;
	size_t orgpos = nk_ftell(f);
	c = nk_fpeek(f);
	if (c == '"') {
		c = nk_fnext_fast(f);
		status = 1;
		while (c != -1 && c != '"') {
			c = nk_fscan_escape(f);
			if (c == -1)
				break;
			if (buf_size > 1) {
				*buf++ = (char)c;
				--buf_size;
			}
			c = nk_fpeek(f);
		}
		if (c == '"') {
			nk_fnext_fast(f);
		} else {
			status = 0;
		}
	} else {
		while (width && c != -1 && !nk_isspace(c)) {
			if (buf_size > 1) {
				if (quash_case)
					*buf++ = (char)nk_tolower(c);
				else
					*buf++ = (char)c;
				--buf_size;
			}
			--width;
			status = 1;
			c = nk_fnext_fast(f);
		}
	}
	if (status) {
		if (buf_size)
			*buf = 0;
	} else {
		nk_fseek(f, orgpos);
	}
	return status;
}

int nk_fscan_word(nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	return _nk_fscan_word(0, f, buf, width, buf_size);
}

int nk_fiscan_word(nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	return _nk_fscan_word(1, f, buf, width, buf_size);
}

static int _nk_fscan_ident(int quash_case, nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	int status = 0;
	size_t orgpos = nk_ftell(f);
	int c = nk_fpeek(f);
	if (c == '"') {
		c = nk_fnext_fast(f);
		status = 1;
		while (c != -1 && c != '"') {
			c = nk_fscan_escape(f);
			if (c == -1)
				break;
			if (buf_size > 1) {
				*buf++ = (char)c;
				--buf_size;
			}
			c = nk_fpeek(f);
		}
		if (c == '"') {
			nk_fnext_fast(f);
		} else {
			status = 0;
		}
		goto bye;
	} else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c =='_') {
		while (width && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
			if (buf_size > 1) {
				if (quash_case)
					*buf++ = (char)nk_tolower(c);
				else
					*buf++ = (char)c;
				--buf_size;
			}
			--width;
			status = 1;
			c =nk_fnext_fast(f);
		}
	}
	bye:
	if (status) {
		if (buf_size)
			*buf = 0;
	} else {
		nk_fseek(f, orgpos);
	}
	return status;
}

int nk_fscan_ident(nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	return _nk_fscan_ident(0, f, buf, width, buf_size);
}

int nk_fiscan_ident(nkinfile_t *f, char *buf, int width, size_t buf_size)
{
	return _nk_fscan_ident(1, f, buf, width, buf_size);
}

int nk_fscan_hex(nkinfile_t *f, uint64_t *val, int width)
{
	int c;
	uint64_t v = 0;
	int status = 0;
	size_t orgpos = nk_ftell(f);
	c = nk_fpeek(f);
	while (width && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
		status = 1;
		if (c >= '0' && c <= '9')
			v = v * 16 + (unsigned int)(c - '0');
		else if (c >= 'a' && c <= 'f')
			v = v * 16 + (unsigned int)(c - 'a' + 10);
		else
			v = v * 16 + (unsigned int)(c - 'A' + 10);
		--width;
		c = nk_fnext_fast(f);
	}
	if (status) {
		*val = v;
	} else {
		nk_fseek(f, orgpos);
	}
	return status;
}

int nk_fscan_dec(nkinfile_t *f, uint64_t *val, int width)
{
	int c;
	int inv = 0;
	uint64_t v = 0;
	int status = 0;
	size_t orgpos = nk_ftell(f);
	c = nk_fpeek(f);
	if (c == '-') {
		inv = 1;
		c = nk_fnext_fast(f);
	}
	while (c >= '0' && c <= '9' && width) {
		status = 1;
		v = v * 10 + (unsigned int)(c - '0');
		--width;
		c = nk_fnext_fast(f);
	}
	if (inv)
		v = -v;
	if (status) {
		*val = v;
	} else {
		nk_fseek(f, orgpos);
	}
	return status;
}

int nk_fscan_double(nkinfile_t *f, double *val)
{
#ifdef NKSCAN_NOFLOAT
	(void)f;
	(void)val;
	return 0;
#else
	return fstrtod(f, val);
#endif
}


// Return true for success, ptr updated
// Return false for syntax error, ptr not updated

static int _nk_vscan(int ignore_case, nkinfile_t *f, const char *fmt, va_list ap)
{
	int status = 0;
	size_t orgpos = nk_ftell(f);

	while (*fmt) {
		if (*fmt == ' ' || *fmt == '\t') {
			// Skip whitespace
			nk_fscan_ws(f);
			++fmt;
		} else if (*fmt == '%') {
			void *arg;
			int size = 0; // Integer argument size: 2 = long long, 1 = long, 0 = int, -1 = short, -2 = char
			int width = -1; // Field width: -1 means any length
			// Field width
			if (fmt[1] >= '0' && fmt[1] <= '9')
				width = 0;
			while (fmt[1] >= '0' && fmt[1] <= '9')
				width += *++fmt - '0';
			// Flags
			for (;;) {
				switch (*++fmt) {
					case 'l': ++size; continue;
					case 'h': --size; continue;
					case 'z': size = 10; continue; // size_t
					case 't': size = 20; continue; // ptrdiff_t
				}
				--fmt;
				break;
			}
			switch (*++fmt) {
				case '%': { // Literal %
					goto lit;
				} case 'u': case 'd': { // Signed number
					uint64_t val;
					arg = (void *)va_arg(ap, void *);
					if (nk_fscan_dec(f, &val, width)) {
						switch (size) {
							case 2: *(unsigned long long *)arg = val; break;
							case 1: *(long *)arg = (long)val; break;
							case 0: *(int *)arg = (int)val; break;
							case -1: *(short *)arg = (short)val; break;
							case -2: *(char *)arg = (char)val; break;
							case 10: *(size_t *)arg = (size_t)val; break;
							case 20: *(ptrdiff_t *)arg = (ptrdiff_t)val; break;
						}
					} else {
						goto bye;
					}
					++fmt;
					break;
				} case 'x': { // Unsigned hex number
					uint64_t val;
					arg = (void *)va_arg(ap, void *);
					if (nk_fscan_hex(f, &val, width)) {
						switch (size) {
							case 2: *(unsigned long long *)arg = val; break;
							case 1: *(unsigned long *)arg = (unsigned long)val; break;
							case 0: *(unsigned int *)arg = (unsigned int)val; break;
							case -1: *(unsigned short *)arg = (unsigned short)val; break;
							case -2: *(unsigned char *)arg = (unsigned char)val; break;
							case 10: *(size_t *)arg = (size_t)val; break;
							case 20: *(ptrdiff_t *)arg = (ptrdiff_t)val; break;
						}
					} else {
						goto bye;
					}
					++fmt;
					break;
				} case 'p': { // Pointer
					uint64_t val;
					arg = (void *)va_arg(ap, void *);
					if (nk_fscan_hex(f, &val, width)) {
						*(void **)arg = (void *)(unsigned long)val;
					} else {
						goto bye;
					}
					++fmt;
					break;
#if 0
				} case 'v': { // Any keyval value
					arg = (void *)va_arg(ap, struct value *);
					if (nk_fscan_keyval(f, (struct value *)arg)) {
					} else {
						goto bye;
					}
					++fmt;
					break;
#endif
#ifndef NKSCAN_NODBASE
				} case 'v': { // Value with specified type
					struct type *ty = (struct type *)va_arg(ap, struct type *);
					arg = (void *)va_arg(ap, void *);
					
					if (nk_fscan_keyval(f, ty, (size_t)arg)) {
					} else {
						goto bye;
					}
					++fmt;
					break;
#endif
				} case 'w': { // Save word in buffer: address and size of buffer provided
					size_t buf_size;
					arg = (void *)va_arg(ap, void *);
					buf_size = (size_t)va_arg(ap, size_t);
					if (!_nk_fscan_word(ignore_case, f, (char *)arg, width, buf_size))
						goto bye;
					++fmt;
					break;
				} case 'i': { // Save identifier in buffer: address and size of buffer provided
					size_t buf_size;
					arg = (void *)va_arg(ap, void *);
					buf_size = (size_t)va_arg(ap, size_t);
					if (!_nk_fscan_ident(ignore_case, f, (char *)arg, width, buf_size))
						goto bye;
					++fmt;
					break;
				} case 'f': case 'g': { // Floating point
					double val;
					arg = (void *)va_arg(ap, void *);
					if (nk_fscan_double(f, &val)) {
						*(double *)arg = val;
					} else {
						goto bye;
					}
					++fmt;
					break;
				} case 'e': { // Succeed even if we are not at end
					++fmt;
					status = 1;
					goto bye;
					break;
				}
			}
		} else if (ignore_case) {
			// Case ignoring literal character match
			if (nk_tolower(nk_fpeek(f)) != nk_tolower(*(const unsigned char *)fmt)) {
				goto bye;
			} else {
				nk_fnext_fast(f);
				++fmt;
			}
		} else {
			// Literal character match
			lit:
			if (nk_fpeek(f) != *(const unsigned char *)fmt) {
				goto bye;
			} else {
				nk_fnext_fast(f);
				++fmt;
			}
		}
	}

	// Skip whitespace?
	// We probbaly almost always want this..
	//nk_vscan_ws(readc, ptr);

	// Succeed only if input is done
	if  (nk_fpeek(f) == -1)
		status = 1;

	bye:
	if (!status)
		nk_fseek(f, orgpos);

	return status;
}

int nk_vscan(nkinfile_t *f, const char *fmt, va_list ap)
{
	return _nk_vscan(0, f, fmt, ap);
}

int nk_viscan(nkinfile_t *f, const char *fmt, va_list ap)
{
	return _nk_vscan(1, f, fmt, ap);
}

int nk_fscan(nkinfile_t *f, const char *fmt, ...)
{
	int status;
	va_list ap;
	va_start (ap, fmt);
	status = nk_vscan(f, fmt, ap);
	va_end(ap);
	return status;
}

int nk_fiscan(nkinfile_t *f, const char *fmt, ...)
{
	int status;
	va_list ap;
	va_start (ap, fmt);
	status = nk_viscan(f, fmt, ap);
	va_end(ap);
	return status;
}
