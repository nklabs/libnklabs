// INA219 bidirectional I2C current/power/voltage monitor

#ifndef _Inkdriver_ina219
#define _Inkdriver_ina219

#include "nki2c.h"

#define INA219_I2C_ADDR 0x40

#define INA219_REG_CONFIG 0x00
#define INA219_REG_VSHUNT 0x01
#define INA219_REG_VBUS 0x02
#define INA219_REG_POWER 0x03
#define INA219_REG_CURRENT 0x04
#define INA219_REG_CAL 0x05

#define RSHUNT 0.1f
#define CURRENT_MAX 3.2f // Amps
#define CURRENT_LSB (CURRENT_MAX / 32768.0f)
#define POWER_LSB (20.0f * CURRENT_LSB)
#define CAL (.04096f / (CURRENT_LSB * RSHUNT))

int nk_ina219_init(const nk_i2c_device_t *dev);

int nk_ina219_read(const nk_i2c_device_t *dev, float *vbus_rtn, float *vshunt_rtn, float *power_rtn, float *current_rtn);

#endif
