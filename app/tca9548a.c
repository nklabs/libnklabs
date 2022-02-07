#include "nkcli.h"
#include "nkdriver_tca9548a.h"
#include "i2c.h"

const nk_i2c_device_t tca9548a =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = TCA9548A_I2C_ADDR + 0 
};

static int cmd_tca9548a(nkinfile_t *args)
{
    uint8_t val;
    if (nk_fscan(args, "")) {
        int rtn = nk_tca9548a_get(&tca9548a, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Bus switch set to %x\n", val);
        }
    } else if (nk_fscan(args, "%hhx ", &val)) {
        int rtn = nk_tca9548a_set(&tca9548a, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Bus switch set to %x\n", val);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tca9548a,
    ">tca9548a                  TCA9548A commands\n"
    "-tca9548a [xx]             Show / [Set] bus switch\n"
)
