#ifndef _Inkrtc
#define _Inkrtc

void nk_rtc_init();
//int cmd_hwclock(char *args);
void date_string(char *buf); // 20 characters plus NUL
int set_datetime(int year, int month, int day, int weekday, int hour, int min, int sec);
void get_datetime(int *year, int *month, int *day, int *weekday, int *hour, int *min, int *sec);

// Current minute of day: 0 - 1439
extern int time_correction;
int get_minute_of_day();
int get_corrected_minute_of_day();

// Get time of week in minutes since midnight Sunday
// So this returns 0 .. 10079
int get_minute_of_week();

#define MINS_PER_DAY 1440
#define MINS_PER_WEEK 10080

#endif
