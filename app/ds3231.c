#include "nkdriver_ds3231.h"
#include "nkrtc.h"

const nk_i2c_device_t ds3231_device =
{
    .i2c_bus = &ard_I2c_bus, // Which bus it's on
    .i2c_addr = DS3231_I2C_ADDR // I2C address of device
};

const nk_rtc_t ds3231 =
{
    .set_datetime = nk_ds3231_set_datetime,
    .get_datetime = nk_ds3231_get_datetime,
    .port = &ds3231_device
};

int cmd_ds3231(nkinfile_t *args)
{
    return nk_ext_rtc_command(&ds3231, args);
}

COMMAND(cmd_ds3231,
	">ds3231                    External RTC access\n"
        "-ds3231                    Show date/time\n"
        "-ds3231 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
