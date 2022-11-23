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

// A SPI device
//   A combination of chip select pin and SPI device driver handle

#ifndef _Inkspi
#define _Inkspi

#include <stdint.h>
#include "nkarch.h"
#include "nkpin.h"

#ifdef NK_PLATFORM_STM32

void shared_gpio_setup_for_spi(void); // For STM32 boards where USER_LED is on the same pin as MAIN_SPI_CLK
void shared_gpio_setup_for_led(void); // For STM32 boards...

typedef struct {
    const nk_pin_t *cs_pin; // Pin of CS_L line
    SPI_HandleTypeDef *hspi; // SPI driver handle
} nkspi_device_t;

#endif

#ifdef NK_PLATFORM_ATSAM

typedef struct {
    const nk_pin_t *cs_pin; // Pin of CS_L line
    struct spi_m_sync_descriptor *hspi; // SPI device handle
} nkspi_device_t;

#endif

#ifdef NK_PLATFORM_ZYNQMP

typedef struct {
    const nk_pin_t *cs_pin; // Pin of CS_L line
    //struct spi_m_sync_descriptor *hspi; // SPI device handle
} nkspi_device_t;

#endif

// Perform a SPI transaction

int nk_spi_transfer(const nkspi_device_t *info, uint8_t *date, uint32_t len);

#endif
