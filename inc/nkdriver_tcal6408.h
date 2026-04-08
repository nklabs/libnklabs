#ifndef _Inkdriver_tcal6408
#define _Inkdriver_tcal6408

#include "nki2c.h"

#define TCAL6408_ADDR 0x20

#define TCAL6408_INPUT_PORT 0x00
#define TCAL6408_OUTPUT_PORT 0x01
#define TCAL6408_POLARITY_PORT 0x02
#define TCAL6408_CONFIGURATION_PORT 0x03

int tcal6408_get_input(const nk_i2c_device_t *dev, uint8_t pin, uint8_t *result);
int tcal6408_set_output(const nk_i2c_device_t *dev, uint8_t pin, uint8_t val);
int tcal6408_read(const nk_i2c_device_t *dev, uint8_t addr, uint8_t *result);
int tcal6408_write(const nk_i2c_device_t *dev, uint8_t addr, uint8_t val);
int bin2dec(const char bits[], uint8_t *result);
int hex2dec(const char hex[], uint8_t *result);

#endif
