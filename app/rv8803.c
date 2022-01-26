#include "nkdriver_rv8803.h"
#include "nkrtc.h"
#include "i2c.h"

const nk_i2c_device_t rv8803_device =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = RV8803_I2C_ADDR // I2C address of device
};

const nk_rtc_t rv8803 =
{
    .set_datetime = (int (*)(const void *, const nkdatetime_t *))nk_rv8803_set_datetime,
    .get_datetime = (int (*)(const void *, nkdatetime_t *))nk_rv8803_get_datetime,
    .port = &rv8803_device
};

int cmd_rv8803(nkinfile_t *args)
{
    return nk_rtc_command(&rv8803, args);
}

COMMAND(cmd_rv8803,
	">rv8803                    External RTC access\n"
        "-rv8803                    Show date/time\n"
        "-rv8803 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
