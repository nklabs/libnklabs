// INA219 bidirectional I2C current/power/voltage monitor

#include "nkcli.h"
#include "nki2c.h"
#include "nkdriver_ina219.h"
#include "i2c.h"

const nk_i2c_device_t ina219 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = INA219_I2C_ADDR
};

static int cmd_ina219(nkinfile_t *args)
{
    float vbus, vshunt, power, current;
    if (nk_fscan(args, "")) {
        int rtn = nk_ina219_read(&ina219, &vbus, &vshunt, &power, &current);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("VBUS = %g V\n", vbus);
            nk_printf("VSHUNT = %g V\n", vshunt);
            nk_printf("POWER = %g W\n", power);
            nk_printf("CURRENT = %g A\n", current);
        }
    } else if (nk_fscan(args, "init ")) {
        int rtn = nk_ina219_init(&ina219);
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
