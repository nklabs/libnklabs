#ifndef _Inkrtc
#define _Inkrtc

// Interface to on-die MCU RTC
void nk_mcu_rtc_init();
int nk_mcu_rtc_set_datetime(int year, int month, int day, int hour, int min, int sec);
int nk_mcu_rtc_get_datetime(int *year, int *month, int *day, int *hour, int *min, int *sec);

// Utilities
int nk_mcu_rtc_date_string(char *buf, size_t len); // 19 characters plus NUL: YYYY-MM-DD HH:MM:SS

// "Sun", "Mon", "Tue", etc.
extern const char *nk_day_name[7];

// Compute day of week [0 .. 6] from date
int nk_cal_day_of_week(int y, int m, int d);

// Get current minute of day [0 .. 1439]
int nk_mcu_rtc_get_minute_of_day(void);

// Get current minute of week [0 .. 10,079]
int nk_mcu_get_minute_of_week(void);

#endif
