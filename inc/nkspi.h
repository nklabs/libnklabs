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

// Perform a SPI transaction

int nk_spi_transfer(const nkspi_device_t *info, uint8_t *date, uint32_t len);

#endif
