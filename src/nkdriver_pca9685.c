// PCA9685 PWM LED modulator
//  Also this is a good chip to drive model RC plane servos

#include "nkdriver_pca9685.h"

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
