# Real Time Clock functions

See [nkdatetime.md](nkdatetime.md) for the definition of nkdatetime_t.

## Files

[nki2crtc_ds3231.c](../src/nki2crtc_ds3231.c),
[nki2crtc_pcf8523.c](../src/nki2crtc_pcf8523.c),
[nketxrtc.h](../inc/nkextrtc.h),
[nkmcurtc.h](../inc/nkmcurtc.h)

## Access external Real Time Clock

~~~c
int nk_ext_rtc_init(void *port);
int nk_ext_rtc_set_datetime(void *port, const nkdatetime_t *datetime);
int nk_ext_rtc_get_datetime(void *port, nkdatetime_t *datetime);
~~~

__port__ is the handle for the I2C bus used by the Real Time Clock IC.

These functions all return 0 for success.

Call nk_ext_rtc_init once at startup to initialize the chip.  If the real
time clock has an invalid date or time, the chip is reset.

There are a number of different drivers that provide these functions.  Just
one of them should be included in the project:

* nki2crtc_pcf8523 - for the PCF8523 Real Time Clock IC
* nki2crtc_ds3231 - for the DS3231 Real Time Clock IC

## nk_ext_rtc_command

~~~c
int nk_ext_rtc_command(void *port, nkinfile_t *args);
~~~

nk_ext_rtc_command provides a command line user interface for a real time
clock IC.  __args__ is parsed for the following commands:

* With no arguments: print the date / time
* YYYY-MM-DD HH:MM:SS: Set date / time.  Day of week is calculated from date.

## Access MCU's Real Time Clock

~~~c
int nk_mcu_rtc_init(void);
int nk_mcu_rtc_set_datetime(const nkdatetime_t *datetime);
int nk_mcu_rtc_get_datetime(nkdatetime_t *datetime);
~~~

Note that the ATSAM integrated real time clock does not have a weekday
support.  So for ATSAM devices: Weekday is ignored in nk_mcu_rtc_set_date
and is set to 0 when nk_mcu_rtc_get_date is called.  The weekday can be
calculated from the date by calling nk_calc_day_of_week.

These functions all return 0 for success.

Call nk_mcu_rtc_init once at startup to initialize the MCU's real time
clock.

## MCU Real Time Clock CLI

A command line user interface is provided for the MCU's real time clock. 
The following commands are provided:

* mcurtc   Print the date/time
* mcurtc YYYY-MM-DD HH:MM:SS  Set the date / time.
