#include "nkdriver_pcf8523.h"
#include "i2c.h"
#include "nkrtc.h"

const nk_i2c_device_t pcf8523_device =
{
    .i2c_bus = &ard_I2c_bus, // Which bus it's on
    .i2c_addr = PCF8523_I2C_ADDR // I2C address of device
};

const nk_rtc_t pcf8523 =
{
    .set_datetime = nk_ds3231_set_datetime,
    .get_datetime = nk_ds3231_get_datetime,
    .port = &pcf8523_device
};

int cmd_pcf8523(nkinfile_t *args)
{
    return nk_ext_rtc_command(&ds3231, args);
}

COMMAND(cmd_pcf8523,
	">pcf8523                   External RTC access\n"
        "-pcf8523                   Show date/time\n"
        "-pcf8523 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
