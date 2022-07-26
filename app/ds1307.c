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
