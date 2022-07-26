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

#include "nkstring.h"

int nk_tolower(int a)
{
	if (a >= 'A' && a <= 'Z')
		return a + 'a' - 'A';
	else
		return a;
}

void nk_tolowers(char *s)
{
	int x;
	for (x = 0; s[x]; ++x) {
		s[x] = (char)nk_tolower(s[x]);
	}
}

int nk_stricmp(const char *a, const char *b)
{
	while (*a && *b) {
		if (nk_tolower(*a) == nk_tolower(*b)) {
			++a;
			++b;
		} else {
			break;
		}
	}
	if (!*a && !*b)
		return 0; // Match
	else
		return 1; // No match
}

int nk_memicmp(const char *a, const char *b, size_t len)
{
	while (len) {
		if (nk_tolower(*a) == nk_tolower(*b)) {
			++a;
			++b;
			--len;
		} else {
			break;
		}
	}
	if (!len)
		return 0;
	else if (*a > *b)
		return 1;
	else
		return -1;
}

// We have these because sometimes ctype is not avaiable

int nk_isxdigit(int c)
{
	if ((c >= '0' && c <= '9') ||
	    (c >= 'A' && c <= 'F') ||
	    (c >= 'a' && c <= 'f'))
	    	return 1;
	else
		return 0;
}

int nk_isspace(int c)
{
	if (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
		return 1;
	else
		return 0;
}
