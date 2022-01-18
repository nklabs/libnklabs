#include "nkcli.h"
#include "nkdriver_lm75.h"
#include "i2c.h"

const nk_i2c_device_t lm75 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = LM75_I2C_ADDR
};

static int cmd_lm75(nkinfile_t *args)
{
    if (nk_fscan(args, "")) {
        float val;
        int rtn = nk_lm75_read_temp(&lm75, &val);
        if (rtn) {
            nk_printf("Couldn't access LM75\n");
        } else {
            nk_printf("Temp = %g C (%g F)\n", val, val*1.8f+32.0f);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_lm75,
    ">lm75                      LM75 commands\n"
    "-lm75                      Read temperature\n"
)
