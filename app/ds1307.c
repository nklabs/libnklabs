#include "nkdriver_ds1307.h"
#include "nkrtc.h"
#include "i2c.h"

const nk_i2c_device_t ds1307_device =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = DS1307_I2C_ADDR // I2C address of device
};

const nk_rtc_t ds1307 =
{
    .set_datetime = (int (*)(const void *, const nkdatetime_t *))nk_ds1307_set_datetime,
    .get_datetime = (int (*)(const void *, nkdatetime_t *))nk_ds1307_get_datetime,
    .port = &ds1307_device
};

int cmd_ds1307(nkinfile_t *args)
{
    return nk_rtc_command(&ds1307, args);
}

COMMAND(cmd_ds1307,
	">ds1307                    External RTC access\n"
        "-ds1307                    Show date/time\n"
        "-ds1307 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
