// MCP4725 12-bit I2C DAC

#include "nkcli.h"
#include "nkdriver_mcp4725.h"
#include "i2c.h"

const nk_i2c_device_t mcp4725 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = MCP4725_I2C_ADDR
};

static int cmd_mcp4725(nkinfile_t *args)
{
    uint16_t val;
    if (nk_fscan(args, "%hu ", &val)) {
        int rtn = nk_mcp4725_write(&mcp4725, val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Wrote %d to MCP4725\n", val);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_mcp4725,
    ">mcp4725                   MCP4725 commands\n"
    "-mcp4725 nnn               Write DAC 0 - 4095\n"
)
