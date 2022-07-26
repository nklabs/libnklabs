# Formatted output

## Files

[nkprintf.h](../inc/nkprintf.h),
[nkprintf.c](../src/nkprintf.c),
[nkprintf_fp.c](../src/nkprintf_fp.c)

## Description

```c
int nk_printf(const char *fmt,...);

int nk_fprintf(nkoutfile_t *f, const char *fmt, ...);

int nk_snprintf(char *dest, size_t len, const char *fmt,...);
int nk_sfprintf(char *dest, size_t len, const char *fmt,...);

int nk_indent(int ind);

int nk_findent(nkoutfile_t *f, int ind);

int nk_byte_hex_dump(unsigned char *buf, unsiged long base, unsigned long offset, unsigned long count);

int nk_fbyte_hex_dump(nkoutfile_t *f, unsigned char *buf, unsigned long base, unsigned long offset, unsigned long count);

```

These functions provide low memory overhead formatted printing.  They
provide a subset of the C-standard printf, but use less memory space. 
Floating point is optionally supported, and even when enabled, much less
space will be used than newlib printf.

__nk_printf__ prints to __nkstdout__ using __nk_fputc__.

__nk_fprintf__ prints to the __nkoutfile_t__ specified in the __f__
argument using __nk_fputc__.

__nk_printf__ and __nk_fprintf__ return the bitwise-OR of the return values
of all calls made to __nk_fputc__.  A return value of 0 means no error.

+__nk_snprintf__ prints a C-string to a memory buffer.  The terminating NUL
is always included.  If the memory area overflows, __nk_snprintf__ returns
true (otherwise false is returned).  Even if the buffer overflows, the
terminating NUL is written.

__nk_sfprintf__ prints a space-filled field to a memory buffer.  It prints
to the specified memory buffer, and then the balance of the buffer is filled
with spaces.  If the memory area overflows, __nk_sfprintf__ returns true,
otherwise false is returned.

__nk_indent__ prints __ind__ spaces to the __nkstdout__ using __nk_fputc__.

__nk_findent__ prints __ind__ spaces to the specified nkoutfile_t using
__nk_fputc__.

__nk_indent__ and __nkfindent__ return the bitwise-OR of the return values
of all calls made to __nk_fputc__.  A return value of zero means no error.

__nk_byte_hex_dump__ prints a hex/ASCII dump to __nkstdout__ using
__nk_fputc__.  __nk_fbyte_hex_dump__ is the same, except the output is to
the specified __nkoutfile_t__.

__count__ is the number of bytes to dump.

__offset__ is the starting byte offset within the __buf__ to dump.

__base__ is added to __offset__ for the printed address.

__nk_byte_hex_dump__ and __nk_fbyte_hex_dump__ return the bit-wise OR of the
return values of all calls to __nk_fputc__.  A return value of 0 means no
error.

## Format string reference

All characters are printed as-is, except for '%' which introduces a format
specifier.  Use '%%' to print '%'.  The last character of the format
specifier should be one of these:

* c prints a character
* s prints a C string
* d prints an int in decimal
* u prints an unsigned int in decimal
* x prints an unsigned int in lower case hexadecimal
* X print an unsigned int in upper case hexadecimal
* p prints a pointer
* f prints a double precision floating point number: 0.000120
* e, E prints a double precision floating point number: 1.200000e-4, 1.200000E-4
* g, G prints a double precision floating point number: 0.0012

Flag characters may optionally appear after the %:

* '-' Left justifies text within field, otherwise right justify
* '+' A plus sign or minus sign prefixes the number
* ' ' A space prefixes a positive number
* '#' Alternate form (hexadecimal prefixed with 0x).
* '0' Zero pad to the left

Next, a field width may optionally appear (NNN).  '*' may be given instead-
it means the field width is given as the integrer argument.

Next, a period followed by a precision may optionally appear (.NNN).  '.*'
may be given instead- it means the precision is given as an integer
argument.

For integer formats, an int or unsigned int argument is assumed, but this
can be overridden:

* hh for a char
* h for a short
* l for a long
* ll for a long long
 