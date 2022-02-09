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
