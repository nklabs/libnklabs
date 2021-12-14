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
#include "nkcli.h"
#include "nkextrtc.h"

// User interface

int nk_ext_rtc_command(void *port, nkinfile_t *args)
{
    int year, month, day, hour, min, sec;
    if (nk_fscan(args, "%d-%d-%d %d:%d:%d ", &year, &month, &day, &hour, &min, &sec)) {
        int rtn = nk_ext_rtc_set_datetime(port, year, month, day, hour, min, sec);
        if (rtn) {
            nk_printf("Error getting date/time %d\n", rtn);
        } else {
            nk_printf("Time and date set.\n");
        }
    } else if (nk_fscan(args, "%e")) {
        int rtn = nk_ext_rtc_get_datetime(port, &year, &month, &day, &hour, &min, &sec);
        if (rtn) {
            nk_printf("Error setting date/time %d\n", rtn);
        } else {
            nk_printf("%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d\n", year, month, day, hour, min, sec);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}
