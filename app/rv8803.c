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
