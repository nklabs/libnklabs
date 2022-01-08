#include "nkcli.h"
#include "nki2c.h"

#define LM75_I2C_ADDR 0x48
#define LM75_PORT (&ARD_I2C)

int nk_lm75_read_temp(float *val)
{
    int rtn;
    uint16_t rdata = 0;
    int16_t v;
    rtn = nk_i2c_get_beshort(LM75_PORT, LM75_I2C_ADDR, 0, &rdata);
    v = (int16_t)rdata;
    if (v < 0) {
        // Avoid signed right shift
        *val = -((-v >> 7)) * 0.5f;
    } else {
        *val = (v >> 7) * 0.5f;
    }
    return rtn;
}
static int cmd_lm75(nkinfile_t *args)
{
    if (nk_fscan(args, "")) {
        float val;
        int rtn = nk_lm75_read_temp(&val);
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
