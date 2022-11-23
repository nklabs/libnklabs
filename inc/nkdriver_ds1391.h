#ifndef _nkdriver_ds1391
#define _nkdriver_ds1391

#include "nkdatetime.h"
#include "nkspi.h"

int nk_ds1391_set_datetime(const nkspi_device_t *dev, const nkdatetime_t *datetime);
int nk_ds1391_get_datetime(const nkspi_device_t *dev, nkdatetime_t *datetime);

#endif
