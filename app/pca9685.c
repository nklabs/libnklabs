// PCA9685 PWM LED modulator
//  Also this is a good chip to drive model RC plane servos

#include "nkcli.h"
#include "nkdriver_pca9685.h"
#include "i2c.h"

const nk_i2c_device_t pca9685 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = PCA9685_I2C_ADDR
};

static int cmd_pca9685(nkinfile_t *args)
{
    int rtn;
    double freq;
    int led;
    double pwm;
    double pos;
    if (nk_fscan(args, "")) {
    } else if (nk_fscan(args, "init ")) {
        rtn = nk_pca9685_setup(&pca9685, 50.0f);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9685 Set up for 50 Hz\n");
        }
    } else if (nk_fscan(args, "init %f", &freq)) {
        rtn = nk_pca9685_setup(&pca9685, freq);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9685 Set up for %g Hz\n", freq);
        }
    } else if (nk_fscan(args, "pwm %d %f ", &led, &pwm)) {
        rtn = nk_pca9685_set(&pca9685, led, pwm);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9685 channel %d set to %g%%\n", led, pwm/100.0);
        }
    } else if (nk_fscan(args, "servo %d %f ", &led, &pos)) {
        rtn = nk_pca9685_set(&pca9685, led, .075 + .025*pos/45.0);
        if (rtn) {
            nk_printf("I2C error\n");
        } else {
            nk_printf("PCA9685 channel %d set to %g degrees\n", led, pos);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pca9685,
    ">pca9685                   PCA9685 commands\n"
    "-pca9685 init [hz]         Initialize for specific PWM frequency (default = 50 Hz)\n"
    "-pca9685 pwm nn pp         Set channel nn (0 - 15) to a specific PWM duty cycle (0 - 100)\n"
    "-pca9685 servo nn pos      Set servo nn (0 - 15) to position (-45 to 45 degrees)\n"
)
