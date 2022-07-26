# Date and time functions

## Files

[nkdatetime.h](../inc/nkdatetime.h), [nkdatetime.c](../src/nkdatetime.c)

## nkdatetime_t

This structure can be used to pass around the date and time:

~~~c
typedef struct {
    uint8_t sec; // 0 - 59
    uint8_t min; // 0 - 59
    uint8_t hour; // 0 - 23
    uint8_t weekday; // 0 - 6 (yes, 0 based)
    uint8_t day; // 0 - 30 (yes, 0 based)
    uint8_t month; // 0 - 11 (yes, 0 based)
    uint16_t year; // 2000 - 2099
} nkdatetime_t;
~~~

## nk_weekday_name

~~~c
extern const char *nk_weekday_name[7];
~~~

An array with weekday names: "Sun", "Mon", "Tue", etc.

## nk_month_name

~~~c
extern const char *nk_month_name[12];
~~~

An array with month names: "Jan", "Feb", "Mar", etc.

## nk_date_string

~~~c
int nk_date_string(char *buf, size_t len, const nkdatetime_t *datetime);
~~~

Print nkdatetime_t as a NUL-terminated string in the following format: YYYY-MM-DD HH:MM:SS

## nk_calc_day_of_week

~~~c
int nk_calc_day_of_week(int y, int m, int d);
~~~

Compute day of week [0 .. 6] from date.

## nk_calc_minute_of_day

~~~c
int nk_calc_minute_of_day(const nkdatetime_t *datetime);
~~~

Get current minute of day [0 .. 1439]

## nk_calc_minute_of_week

~~~c
int nk_calc_minute_of_week(const nkdatetime_t *datetime);
~~~

Get current minute of week [0 .. 10,079]

## nk_datetime_sanity

~~~c
int nk_datetime_sanity(nkdatetime_t *datetime);
~~~

Sanity check datetime.  If any problems, make nkdatetime_t have a legal
value and return -1.  If nkdatetime_t was OK, return 0.

## nk_clear_datetime

~~~c
void nk_clear_datetime(nkdatetime_t *datetime);
~~~

Clear nkdatetime_t to a valid value: Jan 1, 2000.

## nk_datetime_to_unix

~~~c
long long nk_datetime_to_unix(const nkdatetime_t *datetime);
~~~

Convert from nkdatetime_t to UNIX time (number of seconds from Jan 1, 1970). 
Returns -1 for error.

This is for local time only- there is no concept of timezone.

## nk_unix_to_datetime

~~~c
int nk_unix_to_datetime(long long unixtime, nkdatetime_t *datetime);
~~~

Convert from UNIX time (number of seconds from Jan 1, 1970) to nkdatetime_t. 
Return 0 for success.

This is for local time only- there is no concept of timezone.

