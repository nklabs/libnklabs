// This is the floating point formatter extracted from Walter Bright's
// Zortech-C compiler (now called "Digital Mars C").  We use this because
// it is much smaller than the newlib one.

//_ fmt.c
// Copyright (C) 1986-2009 by Digital Mars
// All Rights Reserved
// http://www.digitalmars.com

/*
All the files in this package that are copyrighted by:

    Walter Bright
    Digital Mars
    Symantec
    SLR Systems
    D Language Foundation

are licensed by the Boost Software License:

http://www.boost.org/users/license.html

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#include	<stdlib.h>
#include	<ctype.h>
#include	<float.h>
#include	<string.h>
#include	<math.h>
#include	<errno.h>

#include "nkstrtod.h"
#include "nkstring.h"
#include "nkdectab.h"

/*************************
 * Convert string to double.
 * Terminates on first unrecognized character.
 */

int fstrtod(nkinfile_t *f, double *rtn)
{
	double dval;
	int exp;
	unsigned long msdec,lsdec;
	unsigned long msscale;
	char dot,sign,subject;
	int pow;
	
	while (isspace(nk_fpeek(f)))
		nk_fnext_fast(f);

	sign = 0;			/* indicating +			*/
	switch (nk_fpeek(f))
	{	case '-':
			sign++;
			/* FALL-THROUGH */
		case '+':
			nk_fnext_fast(f);
	}
	subject = 0;
	dval = 0.0;
	dot = 0;			/* if decimal point has been seen */
	exp = 0;
	msdec = lsdec = 0;
	msscale = 1;

	if (nk_tolower(nk_fpeek(f)) == 'i')
	{
		nk_fnext_fast(f);
		if (nk_tolower(nk_fpeek(f)) == 'n')
		{
			nk_fnext_fast(f);
			if (nk_tolower(nk_fpeek(f)) == 'f')
			{
				nk_fnext_fast(f);
				dval = HUGE_VAL;
				subject = 1;
				goto bye;
			}
		}
		return 0;
	}

	if (nk_tolower(nk_fpeek(f)) == 'n')
	{
		nk_fnext_fast(f);
		if (nk_tolower(nk_fpeek(f)) == 'a')
		{
			nk_fnext_fast(f);
			if (nk_tolower(nk_fpeek(f)) == 'n')
			{
				nk_fnext_fast(f);
				dval = NAN;
				subject = 1;
				goto bye;
			}
		}
		return 0;
	}

	{
	    while (1)
	    {	int i = nk_fpeek(f);

		while (isdigit(i))
		{
		    subject = 1;	/* must have at least 1 digit	*/
		    if (msdec < (0x7FFFFFFF-10)/10)
			msdec = msdec * 10 + (i - '0');
		    else if (msscale < (0x7FFFFFFF-10)/10)
		    {	lsdec = lsdec * 10 + (i - '0');
			msscale *= 10;
		    }
		    else
			exp++;
		    exp -= dot;
		    nk_fnext_fast(f);
		    i = nk_fpeek(f);
		}
		if (i == '.' && !dot)
		{	nk_fnext_fast(f);
			dot++;
		}
		else
			break;
	    }
	}
	if (!subject)			/* if error (no digits seen)	*/
	    return 0;
	if (nk_tolower(nk_fpeek(f)) == 'e')
	{
	    char sexp;
	    int e;
	    sexp = 0;
	    nk_fnext_fast(f);
	    switch (nk_fpeek(f))
	    {	case '-':
			sexp++;
			/* FALL-THROUGH */
		case '+':
			nk_fnext_fast(f);
	    }
	    subject = 0;
	    e = 0;
	    while (isdigit(nk_fpeek(f)))
	    {
		if (e < DBL_MAX_EXP*2)		/* prevent integer overflow */
		    e = e * 10 + nk_fpeek(f) - '0';
		nk_fnext_fast(f);
		subject = 1;
	    }
	    exp += (sexp) ? -e : e;
	    if (!subject)		/* if no digits in exponent	*/
	    	return 0;
	}

	dval = msdec;
	if (msscale != 1)		/* if stuff was accumulated in lsdec */
	    dval = dval * msscale + lsdec;

	if (dval)
	{   unsigned u;
	    u = 0;
	    pow = 256;

	    while (exp > 0)
	    {	while (exp >= pow)
		{	dval *= postab[u];
			exp -= pow;
		}
		pow >>= 1;
		u++;
	    }
	    while (exp < 0)
	    {	while (exp <= -pow)
		{	dval *= negtab[u];
			if (dval == 0)
				errno = ERANGE;
			exp += pow;
		}
		pow >>= 1;
		u++;
	    }
	        
	    /* if overflow occurred		*/
	    if (dval == HUGE_VAL)
		errno = ERANGE;		/* range error			*/
	}

    bye:
    
    *rtn = (sign ? -dval : dval);

    return subject;
}
