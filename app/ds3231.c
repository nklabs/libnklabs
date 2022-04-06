// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "nkdriver_ds3231.h"
#include "nkrtc.h"
#include "i2c.h"

const nk_i2c_device_t ds3231_device =
{
    .i2c_bus = &ard_i2c_bus, // Which bus it's on
    .i2c_addr = DS3231_I2C_ADDR // I2C address of device
};

const nk_rtc_t ds3231 =
{
    .set_datetime = (int (*)(const void *, const nkdatetime_t *))nk_ds3231_set_datetime,
    .get_datetime = (int (*)(const void *, nkdatetime_t *))nk_ds3231_get_datetime,
    .port = &ds3231_device
};

int cmd_ds3231(nkinfile_t *args)
{
    if (nk_fscan(args, "temp "))
    {
        float temp;
        if (nk_ds3231_get_temp(&ds3231_device, &temp))
            nk_printf("I2C error\n");
        else
            nk_printf("%g C\n", temp);
        return 0;
    }
    else
    {
        return nk_rtc_command(&ds3231, args);
    }
}

COMMAND(cmd_ds3231,
	">ds3231                    External RTC access\n"
        "-ds3231                    Show date/time\n"
        "-ds3231 temp               Show DS3231 temperature\n"
        "-ds3231 YYYY-MM-DD HH:MM:SS\n"
        "-                          Set date/time\n"
)
