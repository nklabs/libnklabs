// INA219 bidirectional I2C current/power/voltage monitor

#include "nkcli.h"
#include "nki2c.h"
#include "i2c.h"

const nk_i2c_device_t ina219 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = 0x40
};

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

int nk_ina219_init()
{
    uint16_t cal = CAL;
    nk_printf("%x\n", cal);
    return nk_i2c_put_beshort(&ina219, INA219_REG_CAL, CAL);
}

int nk_ina219_read(float *vbus_rtn, float *vshunt_rtn, float *power_rtn, float *current_rtn)
{
    int rtn;
    uint16_t vshunt = 0;
    uint16_t vbus = 0;
    uint16_t power = 0;
    uint16_t current = 0;
    rtn = nk_i2c_get_beshort(&ina219, INA219_REG_VSHUNT, &vshunt);
    rtn |= nk_i2c_get_beshort(&ina219, INA219_REG_VBUS, &vbus);
    rtn |= nk_i2c_get_beshort(&ina219, INA219_REG_POWER, &power);
    rtn |= nk_i2c_get_beshort(&ina219, INA219_REG_CURRENT, &current);

    *vshunt_rtn = (10e-6f * (float)(int16_t)vshunt); // LSB is 10uV for PGA = /8
    *vbus_rtn = (4e-3f * (float)(vbus >> 3)); // LSB is 4mV
    *current_rtn = CURRENT_LSB * (float)(int16_t)current;
    *power_rtn = POWER_LSB * (float)power;

    return rtn;
}

static int cmd_ina219(nkinfile_t *args)
{
    float vbus, vshunt, power, current;
    if (nk_fscan(args, "")) {
        int rtn = nk_ina219_read(&vbus, &vshunt, &power, &current);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("VBUS = %g V\n", vbus);
            nk_printf("VSHUNT = %g V\n", vshunt);
            nk_printf("POWER = %g W\n", power);
            nk_printf("CURRENT = %g A\n", current);
        }
    } else if (nk_fscan(args, "init ")) {
        int rtn = nk_ina219_init();
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Calibration value installed\n");
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_ina219,
    ">ina219                    INA219 commands\n"
    "-ina219                    Read voltage/current/power\n"
)
