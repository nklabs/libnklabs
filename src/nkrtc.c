// Copyright 2020 NK Labs, LLC

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

#include <string.h>
#include "nkrtc.h"

// User interface

int nk_rtc_command(const nk_rtc_t *dev, nkinfile_t *args)
{
    nkdatetime_t datetime;
    if (nk_fscan(args, "%"PRIu16"-%"PRIu8"-%"PRIu8" %"PRIu8":%"PRIu8":%"PRIu8" ", &datetime.year, &datetime.month, &datetime.day, &datetime.hour, &datetime.min, &datetime.sec)) {
        datetime.month--;
        datetime.day--;
        datetime.weekday = nk_calc_day_of_week(datetime.year, datetime.month, datetime.day);
        nk_printf("%s\n", nk_weekday_name[datetime.weekday]);
        if (nk_datetime_sanity(&datetime))
        {
            nk_printf("Invalid date/time\n");
        }
        else
        {
            int rtn = dev->set_datetime(dev->port, &datetime);
            if (rtn) {
                nk_printf("Error setting date/time %d\n", rtn);
            } else {
                nk_printf("Time and date set.\n");
            }
        }
    } else if (nk_fscan(args, "")) {
        int rtn = dev->get_datetime(dev->port, &datetime);
        if (rtn) {
            nk_printf("Error getting date/time %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", datetime.year, 1 + datetime.month, 1 + datetime.day, datetime.hour, datetime.min, datetime.sec);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}
