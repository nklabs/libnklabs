#include "nkcli.h"
#include "nkdriver_ads1115.h"
#include "i2c.h"

const nk_i2c_device_t ads1115 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = ADS1115_I2C_ADDR
};

static int cmd_ads1115(nkinfile_t *args)
{
    int chan;
    if (nk_fscan(args, "")) {
        int16_t val;
        int rtn = nk_ads1115_read(&ads1115, &val);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("Read %d\n", val);
        }
    } else if (nk_fscan(args, "%d ", &chan)) {
        uint16_t c;
        switch (chan) {
            case 0: c = ADS1115_MUX_AIN0; break;
            case 1: c = ADS1115_MUX_AIN1; break;
            case 2: c = ADS1115_MUX_AIN2; break;
            case 3: c = ADS1115_MUX_AIN3; break;
            default: {
                nk_printf("Invalid channel\n");
                return 0;
            }
        }
        int rtn = nk_ads1115_set_mode(&ads1115,
            ADS1115_COMP_QUE_DISABLE |
            ADS1115_DATA_RATE_128 |
            ADS1115_PGA_2048 |
            c
        );
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("ADS1115 set up for channel %d\n", chan);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_ads1115,
    ">ads1115                   ADS1115 commands\n"
    "-ads1115                   Read conversion result\n"
    "-ads1115 n                 Set up for continuous conversion from input 0 - 3\n"
)
