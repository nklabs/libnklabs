#ifndef _Inkextrtc
#define _Inkextrtc

#include <nkcli.h>

// Driver provides these
int nk_ext_rtc_set_datetime(void *port, int year, int month, int day, int hour, int min, int sec);
int nk_ext_rtc_get_datetime(void *port, int *year, int *month, int *day, int *hour, int *min, int *sec);
int nk_ext_rtc_init(void *port);

// User interface
int nk_ext_rtc_command(void *port, nkinfile_t *args);

#endif
