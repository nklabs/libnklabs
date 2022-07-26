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

#include "nkspi.h"

int nk_spi_transfer(const nkspi_device_t *device, uint8_t *data, uint32_t len)
{
#ifdef NK_PLATFORM_ATSAM
    int rtn;

    struct spi_xfer xfer =
    {
        .txbuf = data,
        .rxbuf = data,
        .size = len
    };

    nk_pin_write(device->cs_pin, 0);

    rtn = spi_m_sync_transfer(device->hspi, &xfer);

    nk_pin_write(device->cs_pin, 1);

    return rtn != len; // True (failure) if lengths don't match
#endif

#ifdef NK_PLATFORM_STM32
    shared_gpio_setup_for_spi(); // For STM32 boards where USER_LED is on the same pin as MAIN_SPI_CLK

    nk_pin_write(device->cs_pin, 0); // CS_L

    HAL_SPI_TransmitReceive(device->hspi, data, data, len, 1000);

    nk_pin_write(device->cs_pin, 1); // CS_L

    shared_gpio_setup_for_led(); // For STM32 boards...

    return 0;
#endif
}
