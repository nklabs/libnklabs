#include "nkspi.h"

int nk_spi_transfer(nkspi_device_t *device, uint8_t *data, uint32_t len)
{
#ifdef NK_PLATFORM_ATSAM
    int rtn;

    struct spi_xfer xfer =
    {
        .txbuf = data,
        .rxbuf = data,
        .size = len
    };

    gpio_set_pin_level(device->cs_pin, false);

    rtn = spi_m_sync_transfer(device->hspi, &xfer);

    gpio_set_pin_level(device->cs_pin, true);

    return rtn != len; // True (failure) if lengths don't match
#endif

#ifdef NK_PLATFORM_STM32
    shared_gpio_setup_for_spi(); // For STM32 boards where USER_LED is on the same pin as MAIN_SPI_CLK

    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 0); // CS_L

    HAL_SPI_TransmitReceive(device->hspi, data, data, len, 1000);

    HAL_GPIO_WritePin(device->cs_port, device->cs_pin, 1); // CS_L

    shared_gpio_setup_for_led(); // For STM32 boards...

    return 0;
#endif
}
