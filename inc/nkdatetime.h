#ifndef _Inkdatetime
#define _Inkdatetime

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t sec; // 0 - 59
    uint8_t min; // 0 - 59
    uint8_t hour; // 0 - 23
    uint8_t weekday; // 0 - 6
    uint8_t day; // 1 - 31
    uint8_t month; // 0 - 11
    uint16_t year; // 2000 - 2099
} nkdatetime_t;

// Print date time plus NUL: YYYY-MM-DD HH:MM:SS
int nk_mcu_rtc_date_string(char *buf, size_t len, const nkdatetime_t *datetime);

// "Sun", "Mon", "Tue", etc.
extern const char *nk_weekday_name[7];

// "Jan", "Feb", "Mar", etc.
extern const char *nk_month_name[12];

// Compute day of week [0 .. 6] from date
int nk_calc_day_of_week(int y, int m, int d);

// Get current minute of day [0 .. 1439]
int nk_mcu_rtc_get_minute_of_day(const nkdatetime_t *datetime);

// Get current minute of week [0 .. 10,079]
int nk_mcu_get_minute_of_week(const nkdatetime_t *datetime);

// Sanity check datetime
// If any problems, make datetime have legal value and return -1

int nk_datetime_sanity(nkdatetime_t *datetime);

#endif
