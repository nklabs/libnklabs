// PCA9685 PWM LED modulator
//  Also this is a good chip to drive model RC plane servos

#include "nkcli.h"
#include "nki2c.h"
#include "i2c.h"

#define PCA9685_I2C_ADDR 0x40

const nk_i2c_device_t pca9685 =
{
    .i2c_bus = &ard_i2c_bus,
    .i2c_addr = PCA9685_I2C_ADDR
};

#define PCA9685_REG_MODE1 0x00

// Mode1 bits
#define PCA9685_RESTART_BIT 0x80
#define PCA9685_EXTCLK_BIT 0x40
#define PCA9685_AI_BIT 0x20
#define PCA9685_SLEEP_BIT 0x10
#define PCA9685_SUB1_BIT 0x08
#define PCA9685_SUB2_BIT 0x04
#define PCA9685_SUB3_BIT 0x02
#define PCA9685_ALLCALL_BIT 0x01

#define PCA9685_REG_MODE2 0x01

// Mode2 bits
#define PCA9685_INVRT_BIT 0x10
#define PCA9685_OCH_BIT 0x08
#define PCA9685_OUTDRV_BIT 0x04
// Lower two bits determine output value
// when OE_L input pin is high
#define PCA9685_OUTNE_LOW 0x00
#define PCA9685_OUTNE_HIGH 0x01
#define PCA9685_OUTNE_HZ 0x02

#define PCA9685_REG_SUBADR1 0x02
#define PCA9685_REG_SUBADR2 0x03
#define PCA9685_REG_SUBADR3 0x04
#define PCA9685_REG_ALLCALLADR 0x05

// There are 16 LED outputs.  Multiple LED number by 4 and add to following four registers
// for a specific LED
#define PCA9685_REG_LED0_ON_L 0x06
#define PCA9685_REG_LED0_ON_H 0x07
#define PCA9685_REG_LED0_OFF_L 0x08
#define PCA9685_REG_LED0_OFF_H 0x09

#define PCA9685_REG_ALL_LED_ON_L 0xFA
#define PCA9685_REG_ALL_LED_ON_H 0xFB
#define PCA9685_REG_ALL_LED_OFF_L 0xFC
#define PCA9685_REG_ALL_LED_OFF_H 0xFD
#define PCA9685_REG_PRE_SCALE 0xFE

// Prescaler forumla
//   frequency can range from 1525 Hz to 24 Hz using built-in 25 MHz oscillator
#define PCA9685_PRESCALE_CALC(hz) (25000000.0f / (4096.0f * (hz)) - 1.0f)

// To set PWM frequency 

int nk_pca9685_setup(const nk_i2c_device_t *dev, float hz)
{
    int rtn;

    // Disable oscillator to allow prescale to be written
    rtn = nk_i2c_put_byte(dev, PCA9685_REG_MODE1, PCA9685_AI_BIT | PCA9685_SLEEP_BIT);

    rtn |= nk_i2c_put_byte(dev, PCA9685_REG_PRE_SCALE, PCA9685_PRESCALE_CALC(hz));

    rtn |= nk_i2c_put_byte(dev, PCA9685_REG_MODE1, PCA9685_AI_BIT);

    // Wait for oscillator to turn on
    nk_udelay(1000);

    return rtn;
}

// Set PWM fraction for a channel.  PWM should be between 0.0 and 1.0

int nk_pca9685_set(const nk_i2c_device_t *dev, int led, float pwm)
{
    uint8_t data[5];
    int off = (int)(4096.0f * pwm);
    if (off < 0) off = 0;
    if (off > 4095) off = 4095;

    data[0] = PCA9685_REG_LED0_ON_L + led * 4;
    // Turn on at time = 0
    data[1] = 0;
    data[2] = 0;
    // Turn off
    data[3] = off;
    data[4] = (off >> 8);

    return nk_i2c_write(dev, 5, data);
}

// Read PWM fraction from a channel

int nk_pca9685_get(const nk_i2c_device_t *dev, int led, float *pwm)
{
    int rtn;
    uint8_t data[5];

    data[0] = PCA9685_REG_LED0_ON_L + led * 4;

    rtn = nk_i2c_write(dev, 1, data);
    rtn |= nk_i2c_read(dev, 4, data + 1);

    *pwm = (float)((data[3] + (data[4] << 8)) & 8191) / 4096.0f;

    return rtn;
}

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
