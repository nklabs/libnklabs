
#include "nkarch.h"
#include "nkcli.h"
#include "nkrtc.h"

#if 0

int ext_rtc_set_time(int year, int month, int day, int hour, int min, int sec)
{
    uint8_t buf[9];


    buf[0] = 0x80; // Write to RTC array
    buf[1] = 0;
    buf[2] = ((sec / 10) << 4) + (sec % 10);
    buf[3] = ((min / 10) << 4) + (min % 10);
    buf[4] = ((hour / 10) << 4) + (hour % 10); // 24 hour mode: bit 6 is 0
    buf[5] = 0x01; // Day of week
    buf[6] = ((day / 10) << 4) + (day % 10);
    buf[7] = ((month / 10) << 4) + (month % 10); // Bit 7 is century
    buf[8] = (((year - 2000) / 10) << 4) + ((year - 2000) % 10);

    if (rtc_spi_transfer(buf, sizeof(buf)))
        return -1;

    return 0;
}

void ext_rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
    uint8_t buf[17];

    memset(buf, 0, sizeof(buf));
    buf[0] = 0x00; // Read from RTC array

    if (rtc_spi_transfer(buf, sizeof(buf))) {
        nk_printf("RTC SPI transfer error\n");
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;
        return;
    }

    if (buf[0x0F] & 0x80) {
        nk_printf("OSF bit was set! Clearing RTC...\n");
        buf[0] = 0x80; // Write to RTC array
        buf[1] = 0; // Tenths
        buf[2] = 0; // Second
        buf[3] = 0; // Minutes
        buf[4] = 0; // Hours
        buf[5] = 0x01; // Day of week
        buf[6] = 0x01; // Day of month
        buf[7] = 0x01; // Month
        buf[8] = 0x00; // Year
        buf[9] = 0x0; // Alarm tenths
        buf[10] = 0x0; // Alarm seconds
        buf[11] = 0x0; // Alarm minutes
        buf[12] = 0x0; // Alarm hours
        buf[13] = 0x01; // Alarm day of month
        buf[14] = 0x00; // Enable oscillator
        buf[15] = 0x00; // Clear OSF
        buf[16] = 0x00; // Disable trickle charger
        if (rtc_spi_transfer(buf, sizeof(buf))) {
            nk_printf("RTC SPI transfer error\n");
        }
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;
        return;
    }

    *sec = (0x0F & buf[2]) + ((0x7 & (buf[2] >> 4)) * 10);
    *min = (0x0F & buf[3]) + ((0x07 & (buf[3] >> 4)) * 10);
    *hour = (0x0F & buf[4]) + ((0x3 & (buf[4] >> 4)) * 10);
    *day = (0x0F & buf[6]) + ((0x03 & (buf[6] >> 4)) * 10);
    *month = (0x0F & buf[7]) + ((0x1 & (buf[7] >> 4)) * 10);
    *year = (0x0F & buf[8]) + ((buf[8] >> 4) * 10) + 2000;

    return;
}

/* Set internal clock from external clock */

int set_from_hwclock()
{
    int year, month, day, hour, min, sec;
    int now;
    startup("Set time from external RTC");
    ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
    now = datetime_to_unix(year, month, day, hour, min, sec);
    if (now != -1) {
        Seconds_set(now);
        nk_printf("RTC DateTime is %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
        return 0;
    } else {
        nk_printf("RTC has invalid date/time, time not set\n");
        return -1;
    }
}

/* Set external clock from internal clock */

int set_to_hwclock()
{
    int year, month, day, hour, min, sec;
    int now;
    now = Seconds_get();
    if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
        nk_printf("Error converting UNIX Epoch time to UTC\n");
    } else {
        if (ext_rtc_set_time(year, month, day, hour, min, sec)) {
            nk_printf("Error writing to battery backed real time clock\n");
        } else {
            nk_printf("Battery backed real time clock has been set\n");
            return 0;
        }
    }
    return -1;
}

void rtc_init()
{
    startup("Real time clock");
    set_from_hwclock();
}

int cmd_hwclock(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (facmode && nk_fscan(args, "-w")) {
        set_to_hwclock();
    } else if (facmode && nk_fscan(args, "")) {
        ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
        nk_printf("External clock reports %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", year, month, day, hour, min, sec);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

#endif

// STM32 built-in RTC

extern RTC_HandleTypeDef hrtc;

void get_datetime(int *year, int *month, int *day, int *weekday, int *hour, int *min, int *sec)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    *year = sDate.Year + 2000;
    *month = sDate.Month;
    *day = sDate.Date;
    *weekday = sDate.WeekDay;

    *hour = sTime.Hours;
    *min = sTime.Minutes;
    *sec = sTime.Seconds;
}

int get_minute_of_day()
{
    int year, month, day, weekday;
    int hour, min, sec;
    get_datetime(&year, &month, &day, &weekday, &hour, &min, &sec);
    return hour * 60 + min;
}

int time_correction;

int get_corrected_minute_of_day()
{
    int min = get_minute_of_day() - time_correction;
    if (min < 0)
        min += 1440;
    return min;
}

int get_minute_of_week()
{
    int year;
    int month;
    int day;
    int weekday;
    int hour;
    int min;
    int sec;
    get_datetime(&year, &month, &day, &weekday, &hour, &min, &sec);
    return (weekday - 1) * 1440 + (hour * 60) + min;
}

int set_datetime(int year, int month, int day, int weekday, int hour, int min, int sec)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    if (year < 2000 || year > 2099)
        return -1;

    if (day < 1 || day > 31)
        return -1;

    if (month < 1 || month > 12)	
        return -1;

    if (hour < 0 || hour > 23)
        return -1;
    
    if (min < 0 || min > 59)
        return -1;
    
    if (sec < 0 || sec > 59)
        return -1;

    sDate.Year = year - 2000; // 0..99
    sDate.Month = month; // BCD?
    sDate.Date = day; // 1..31
    sDate.WeekDay = weekday;

    sTime.Hours = hour;
    sTime.Minutes = min;
    sTime.Seconds = sec;
    sTime.TimeFormat = 0;
    sTime.SubSeconds = 0;
    sTime.SecondFraction = 0;
    sTime.DayLightSaving = 0;
    sTime.StoreOperation = 0;
    
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    return 0;
}

int cmd_date(nkinfile_t *args)
{
    int year, month, day, weekday, hour, min, sec;
    if (nk_fscan(args, "%d-%d-%d %d %d:%d:%d ", &year, &month, &day, &weekday, &hour, &min, &sec)) {
        int sta = set_datetime(year, month, day, weekday, hour, min, sec);
        if (sta) {
            nk_printf("Error setting date/time\n");
        } else {
            nk_printf("Time set\n");
        }
    } else if (nk_fscan(args, "")) {
        get_datetime(&year, &month, &day, &weekday, &hour, &min, &sec);
        nk_printf("%4.4d-%2.2d-%2.2d %d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, weekday, hour, min, sec);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

void date_string(char *buf)
{
    int year, month, day, weekday, hour, min, sec;
    get_datetime(&year, &month, &day, &weekday, &hour, &min, &sec);
    nk_snprintf(buf, 40, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ", year, month, day, hour, min, sec);
}

COMMAND(date,
    "date                      Date/time\n",
    "date                      Print date/time\n"
    "date yyyy-mm-dd hh:mm:ss  Set date/time\n",
    ""
)
