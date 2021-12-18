// A SPI device
//   A combination of chip select pin and SPI device driver handle

#ifndef _Inkspi
#define _Inkspi

#include <stdint.h>
#include "nkarch.h"

#ifdef NK_PLATFORM_STM32

typedef struct {
    GPIO_TypeDef *cs_port; // Port of CS_L line
    uint16_t cs_pin; // Pin of CS_L line
    SPI_HandleTypeDef *hspi; // SPI driver handle
} nkspi_device_t;

#endif

#ifdef NK_PLATFORM_ATSAM

typedef struct {
    uint8_t cs_pin; // Pin of CS_L line
    struct spi_m_sync_descriptor *hspi; // SPI device handle
} nkspi_device_t;

#endif

// Perform a SPI transaction

int nk_spi_transfer(void *info, uint8_t *date, uint32_t len);

#endif
