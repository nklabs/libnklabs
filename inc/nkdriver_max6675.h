#ifndef _Inkdriver_max6675
#define _Inkdriver_max6675

#include "nkspi.h"

// Read temperature from thermocouple in Celsius

int nk_max6675_read_temp(const nkspi_device_t *dev, float *val);

#endif
