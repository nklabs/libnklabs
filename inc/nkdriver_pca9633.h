// Copyright 2021 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// PCA9633 PWM LED modulator
//  This is for LEDs only, not good for servos

#ifndef _Inkdriver_pca9633
#define _Inkdriver_pca9633

#include "nki2c.h"

#define PCA9633_I2C_ADDR_8PIN 0x62 // Fixed to this address
#define PCA9633_I2C_ADDR_10PIN 0x60 // A1, A0 set with pins
#define PCA9633_I2C_ADDR_16PIN 0x00 // 7-bit address set with pins

#define PCA9633_REG_MODE1 0x00

// Mode1 bits
#define PCA9633_AI2_BIT 0x80
#define PCA9633_AI1_BIT 0x40
#define PCA9633_AI0_BIT 0x20
#define PCA9633_SLEEP_BIT 0x10
#define PCA9633_SUB1_BIT 0x08
#define PCA9633_SUB2_BIT 0x04
#define PCA9633_SUB3_BIT 0x02
#define PCA9633_ALLCALL_BIT 0x01

#define PCA9633_REG_MODE2 0x01

// Mode2 bits
#define PCA9633_DMBLNK 0x20 // Set for blinking
#define PCA9633_INVRT_BIT 0x10
#define PCA9633_OCH_BIT 0x08
#define PCA9633_OUTDRV_BIT 0x04
// Lower two bits determine output value
// when OE_L input pin is high
#define PCA9633_OUTNE_LOW 0x00
#define PCA9633_OUTNE_HIGH 0x01
#define PCA9633_OUTNE_HZ 0x02

// PWM outputs: 0 = 0%, 255 = 99.6%
#define PCA9633_REG_PWM0 0x02
#define PCA9633_REG_PWM1 0x03
#define PCA9633_REG_PWM2 0x04
#define PCA9633_REG_PWM3 0x05

// Group brightness: all four LEDs (only when DMBLNK is set)
#define PCA9633_REG_GRP_PWM 0x06

// Blinking freq. (only when DMBLNK is set)
#define PCA9633_REG_GRP_FREQ 0x07

// Two bits for each pin:
#define PCA9633_REG_LEDOUT 0x08
#define PCA9633_LDR_OFF 0x00
#define PCA9633_LDR_FULLY_ON 0x01
#define PCA9633_LDR_PWM 0x02
#define PCA9633_LDR_PWMGRP 0x03

#define PCA9633_REG_SUBADR1 0x09
#define PCA9633_REG_SUBADR2 0x0A
#define PCA9633_REG_SUBADR3 0x0B
#define PCA9633_REG_ALLCALLADR 0x0C

// Set up PCA9633

int nk_pca9633_setup(const nk_i2c_device_t *dev);

// Set PWM fraction for a channel.  PWM should be between 0.0 and 1.0

int nk_pca9633_set(const nk_i2c_device_t *dev, int led, float pwm);

// Read PWM fraction from a channel

int nk_pca9633_get(const nk_i2c_device_t *dev, int led, float *pwm);

#endif
