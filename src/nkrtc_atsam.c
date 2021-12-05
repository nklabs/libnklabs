#include "nkarch.h"
#include "nkcli.h"
#include "nkrtc.h"

int ext_rtc_set_time(int year, int month, int day, int hour, int min, int sec)
{
#if 0
    // SPI RTC chip
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
#else
    // ATSAM
    struct calendar_date_time tim;
    int rtn;

    tim.time.sec = sec;
    tim.time.min = min;
    tim.time.hour = hour;
    tim.date.day = day;
    tim.date.month = month;
    tim.date.year = year;

    rtn = calendar_set_date(&CALENDAR_0, &tim.date);
    rtn = calendar_set_time(&CALENDAR_0, &tim.time);

    return rtn;
#endif
}

int ext_rtc_get_time(int *year, int *month, int *day, int *hour, int *min, int *sec)
{
#if 0
    // SPI RTC chip
    uint8_t buf[17];

    memset(buf, 0, sizeof(buf));
    buf[0] = 0x00; // Read from RTC array

    if (rtc_spi_transfer(buf, sizeof(buf))) {
        printf("RTC SPI transfer error\n");
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;
        return;
    }

    if (buf[0x0F] & 0x80) {
        printf("OSF bit was set! Clearing RTC...\n");
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
            printf("RTC SPI transfer error\n");
        }
        *sec = 0;
        *min = 0;
        *hour = 0;
        *day = 1;
        *month = 1;
        *year = 2000;

        return 0;
    }

    *sec = (0x0F & buf[2]) + ((0x7 & (buf[2] >> 4)) * 10);
    *min = (0x0F & buf[3]) + ((0x07 & (buf[3] >> 4)) * 10);
    *hour = (0x0F & buf[4]) + ((0x3 & (buf[4] >> 4)) * 10);
    *day = (0x0F & buf[6]) + ((0x03 & (buf[6] >> 4)) * 10);
    *month = (0x0F & buf[7]) + ((0x1 & (buf[7] >> 4)) * 10);
    *year = (0x0F & buf[8]) + ((buf[8] >> 4) * 10) + 2000;

    return 0;
#else
    // ATSAM
    struct calendar_date_time tim;
    int rtn;

    rtn = calendar_get_date_time(&CALENDAR_0, &tim);

    *sec = tim.time.sec;
    *min = tim.time.min;
    *hour = tim.time.hour;
    *day = tim.date.day;
    *month = tim.date.month;
    *year = tim.date.year;

    return rtn;
#endif
}

#if 0
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
        printf("RTC DateTime is %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
        return 0;
    } else {
        printf("RTC has invalid date/time, time not set\n");
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
        printf("Error converting UNIX Epoch time to UTC\n");
    } else {
        if (ext_rtc_set_time(year, month, day, hour, min, sec)) {
            printf("Error writing to battery backed real time clock\n");
        } else {
            printf("Battery backed real time clock has been set\n");
            return 0;
        }
    }
    return -1;
}
#endif

void nk_rtc_init()
{
    startup("Real time clock");
    // set_from_hwclock();
}

#if 0

// External RTC

int cmd_hwclock(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (facmode && nk_fscan(args, "-w")) {
        set_to_hwclock();
    } else if (facmode && nk_fscan(args, "%e")) {
        ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
        printf("External clock reports %4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", year, month, day, hour, min, sec);
    } else {
        printf("Syntax error\n");
    }
    return 0;
}

int cmd_date(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    int now;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d %e", &year, &month, &day, &hour, &min, &sec)) {
        now = datetime_to_unix(year, month, day, hour, min, sec);
        if (now < 0) {
            printf("Error converting UTC to UNIX Epoc time\n");
        } else {
            Seconds_set(now);
            printf("Time set\n");
            set_to_hwclock();
        }
    } else if (nk_fscan(args, "%e")) {
        now = Seconds_get();
        if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
            printf("Error converting UNIX Epoch time to UTC\n");
        }
        printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
    } else {
        printf("Syntax error\n");
    }
    return 0;
}
#endif

// Internal RTC

int cmd_date(nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d %e", &year, &month, &day, &hour, &min, &sec)) {
        int rtn = ext_rtc_set_time(year, month, day, hour, min, sec);
        if (rtn) {
            nk_printf("Error %d\n", rtn);
        } else {
            nk_printf("Date set.\n");
        }
    } else if (nk_fscan(args, "%e")) {
        int rtn = ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
        if (rtn) {
            nk_printf("Error %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d UTC\n", year, month, day, hour, min, sec);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(date,
	"date                      RTC access\n",
        "date                      Show date/time\n"
        "date YYYY-MM-DD HH:MM:SS  Set date/time\n",
        ""
)

void date_string(char *buf)
{
    int year, month, day, hour, min, sec;
#if 0
    int now;
    now = Seconds_get();
    if (unix_to_datetime(now, &year, &month, &day, &hour, &min, &sec)) {
        printf("Error converting UNIX Epoch time to UTC\n");
    }
#endif
    year = 2000;
    month = 1;
    day = 1;
    hour = 0;
    min = 0;
    sec = 0;
    int rtn = ext_rtc_get_time(&year, &month, &day, &hour, &min, &sec);
    nk_snprintf(buf, 40, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d ", year, month, day, hour, min, sec);
}
