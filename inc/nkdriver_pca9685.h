// PCA9685 PWM LED modulator
//  Also this is a good chip to drive model RC plane servos

#ifndef _Inkdriver_pca9685
#define _Inkdriver_pca9685

#include "nki2c.h"

#define PCA9685_I2C_ADDR 0x40

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

int nk_pca9685_setup(const nk_i2c_device_t *dev, float hz);

// Set PWM fraction for a channel.  PWM should be between 0.0 and 1.0

int nk_pca9685_set(const nk_i2c_device_t *dev, int led, float pwm);

// Read PWM fraction from a channel

int nk_pca9685_get(const nk_i2c_device_t *dev, int led, float *pwm);

#endif
