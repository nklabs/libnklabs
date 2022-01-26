#include "nkcli.h"
#include "nkdriver_pca9633.h"
#include "i2c.h"

const nk_i2c_device_t pca9633 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = PCA9633_I2C_ADDR_10PIN
};

static int cmd_pca9633(nkinfile_t *args)
{
    int rtn;
    int led;
    double pwm;
    if (nk_fscan(args, "")) {
    } else if (nk_fscan(args, "init ")) {
        rtn = nk_pca9633_setup(&pca9633);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9633 Set up for 50 Hz\n");
        }
    } else if (nk_fscan(args, "pwm %d %f ", &led, &pwm)) {
        rtn = nk_pca9633_set(&pca9633, led, pwm);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9633 channel %d set to %g%%\n", led, pwm/100.0);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pca9633,
    ">pca9633                   PCA9633 commands\n"
    "-pca9633 init              Initialize\n"
    "-pca9633 pwm nn pp         Set channel nn (0 - 3) to a specific PWM duty cycle (0 - 100)\n"
)
