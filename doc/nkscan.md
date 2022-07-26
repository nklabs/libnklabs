# Command Line Argument Parser

## Files

[nkscan.h](../inc/nkscan.h),
[nkscan.c](../src/nkscan.c),
[nkstrtod.c](../src/nkstrtod.c)

## Description

## nk_scan()

```c
int nk_fscan(nkinfile_t *f, const char *fmt, ...);
int nk_fiscan(nkinfile_t *f, const char *fmt, ...);

```

These are enhanced versions of C's scanf().  The main enhancement is that
they either fully parse the input as specified by the format string and
return true, or return false but leave the input nkinfile_t left at its
original position.  This allows you to make multiple __nk_fscan__
calls until a correct match is found.  These were designed as a convenient way to
parse command line arguments.

Since these functions may reposition the nkinfile_t, the nkinfile_t must
support random access.

__nk_fscan__ is case sensitive.

__nk_fiscan__ is case insensitive and unquoted words and identifiers (%w and
%i) are quashed to lower case.

## Format string

Each character in the format string must match the next character from the input string, except:

Space matches zero or more spaces or tabs

%%  Matches %

%hhd   Matches a decimal number, stores result into a char.  A (char *) should be provided.

%hd    Matches a decimal number, sotres result into a short.  A (short *) should be provided.

%d     Matches a decimal number, stores result into an int.  An (int *) should be provided.

%ld    Matches a decimal number, stores result into a long.  A (long *) should be provided.

%lld   Matches a decimal number, stores result into a long long.  A (long long *) should be provided.

%zd    Matches a decimal number, stores result into a size_t.  A (size_t *) should be provided.

%td    Matches a decimal number, stores result into a ptrdiff_t.  A (ptrdiff_t *) should be provided.

%hhx etc.   Same as above, but hexadecimal number into unsigned

%hhu etc.   Same as above, but decimal number into unsigned

%f     Matches a floating point number.  A (double *) should be provided.

%p     Matches a pointer in the form of a hexadecimal number, stores result into a void *.  A (void **) should be provided.

%i     Save identifier in a string buffer.  The identifier is composed of [a-zA-Z_][a-zA-Z_0-9]*  Both the address of the string buffer (char *) and the length of the string buffer (size_t) should be provided.

If the identifier begins with ", then assume it's a C string.  All
characters up to the following " are included in the identifier, and C
string escape sequences are available from within the string.

%w     Save word in a string buffer.  The word is composed of non-whitespace chars followed by whitespace or end of input.  Both the address of the string (char *) and the length of the string (size_t) should be provided.

If the word begins with ", then assume it's a C string.  All characters up
to the following " are included in the word, and C string escape sequences
are available from within the string.

%e     Normally the end of the format string must correspond to the end of the input (otherwise it will fail to parse). 
But if the last thing in the format string is %e, then parse will succeed
even if there is more input available.  This allows you to parse more of the
input with another call to nk_fscan.

A field width may be specified after any %, like %2d.  This will limit the
parsing to the indicated size.  WIth %2d, only the first two digits will be
taken.  For example, you could parse a date such as 20181231 with
"%4d%2d%2d", &year, &month, &day
