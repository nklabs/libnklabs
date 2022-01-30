#ifndef _Inkdriver_aip31068
#define _Inkdriver_aip31068

#include "nki2c.h"
#include "nkdriver_hd44780.h"

#define AIP31068_I2C_ADDR 0x3E

// Control byte format
#define AIP31068_CO 0x80 // 1 = allow another control byte, 0 = this is last one
#define AIP31068_RS 0x40 // 1 = write data, 0 = write command

// Initialize AIP31068

int nk_aip31068_init(const nk_i2c_device_t *dev, bool f_bit, bool n_bit);

// Send a command byte

int nk_aip31068_send_command(const nk_i2c_device_t *dev, uint8_t cmd);

// Send data bytes

int nk_aip31068_send_data(const nk_i2c_device_t *dev, uint8_t *data, int len);

#endif
