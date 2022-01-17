#ifndef _Inkrtc
#define _Inkrtc

#include "nkdatetime.h"
#include "nkcli.h"

// Device descriptor for a real time clock
typedef struct {
    int (*set_datetime)(void *port, const nkdatetime_t *datetime);
    int (*get_datetime)(void *port, nkdatetime_t *datetime);
    void *port;
} nk_rtc_t;

#define NK_ERROR_TIME_LOST -100

// User interface
int nk_rtc_command(const nk_rtc_t *dev, nkinfile_t *args);

#endif
