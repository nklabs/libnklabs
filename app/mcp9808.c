// MCP9808 temperature sensor

#include "nkcli.h"
#include "nki2c.h"
#include "i2c.h"

#define MCP9808_I2C_ADDR 0x18

const nk_i2c_device_t mcp9808 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = MCP9808_I2C_ADDR
};

int nk_mcp9808_read_temp(const nk_i2c_device_t *dev, float *val)
{
    int rtn;
    uint16_t rdata = 0;
    rtn = nk_i2c_get_beshort(dev, 5, &rdata);

    if (rdata & 0x1000)
        rdata |= 0xE000;
    else
        rdata &= 0x1FFF;

    *val = (int16_t)rdata * 0.0625f;
    return rtn;
}

static int cmd_mcp9808(nkinfile_t *args)
{
    if (nk_fscan(args, "")) {
        float val;
        int rtn = nk_mcp9808_read_temp(&mcp9808, &val);
        if (rtn) {
            nk_printf("Couldn't access MCP9808\n");
        } else {
            nk_printf("Temp = %g C (%g F)\n", val, val*1.8f+32.0f);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_mcp9808,
    ">mcp9808                   MCP9808 commands\n"
    "-mcp9808                   Read temperature\n"
)
