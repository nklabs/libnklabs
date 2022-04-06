// Copyright 2021 NK Labs, LLC

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

#ifndef _Inkdatetime
#define _Inkdatetime

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t sec; // 0 - 59
    uint8_t min; // 0 - 59
    uint8_t hour; // 0 - 23
    uint8_t weekday; // 0 - 6
    uint8_t day; // 0 - 30
    uint8_t month; // 0 - 11
    uint16_t year; // 2000 - 2099
} nkdatetime_t;

// Print date time plus NUL: YYYY-MM-DD HH:MM:SS
int nk_date_string(char *buf, size_t len, const nkdatetime_t *datetime);

// "Sun", "Mon", "Tue", etc.
extern const char *nk_weekday_name[7];

// "Jan", "Feb", "Mar", etc.
extern const char *nk_month_name[12];

// Compute day of week [0 .. 6] from date
int nk_calc_day_of_week(int y, int m, int d);

// Get current minute of day [0 .. 1439]
int nk_calc_minute_of_day(const nkdatetime_t *datetime);

// Get current minute of week [0 .. 10,079]
int nk_calc_minute_of_week(const nkdatetime_t *datetime);

// Sanity check datetime
// If any problems, make datetime have legal value and return -1

int nk_datetime_sanity(nkdatetime_t *datetime);

// Clear nkdatetime_t
void nk_datetime_clear(nkdatetime_t *datetime);

// Convert to and from UNIX time
// These return -1 for errors

// These are to be used for localtime- there is no concept of timezone

long long nk_datetime_to_unix(const nkdatetime_t *datetime);

int nk_unix_to_datetime(long long unixtime, nkdatetime_t *datetime);

#endif
