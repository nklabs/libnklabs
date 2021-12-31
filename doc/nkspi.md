# SPI device driver

A simple wrapper around the MCU vendor's HAL SPI-driver which includes the
SPI CS (chip select) GPIO pin.  This driver is usable by SPI-bus device
drivers, such as [nkspiflash.md](nkspiflash.md) and
[nkspirtc.md](nkspirtc.md).

## Files

[nkspi.h](../inc/nkspi.h), [nkspi.c](../src/nkspi.c)

## nkspi_device_t

You initialize a structure containing the MCU vendor's HAL SPI device handle and
the GPIO pins to use for chip select.  Pass an address of this structure to
nk_spi_transfer.

~~~c
// For STM32
typedef struct {
    GPIO_TypeDef *cs_port; // Port of CS_L line
    uint16_t cs_pin; // Pin of CS_L line
    SPI_HandleTypeDef *hspi; // SPI driver handle
} nkspi_device_t;

// For ATSAM
typedef struct {
    uint8_t cs_pin; // Pin of CS_L line
    struct spi_m_sync_descriptor *hspi; // SPI device handle
} nkspi_device_t;

~~~

## nk_spi_trasnfer

~~~c
int nk_spi_transfer(void *info, uint8_t *data, uint32_t len);
~~~

Perform a full-duplex SPI-transaction: assert chip select, write and read
__len__ bytes to/from the SPI device, then finally release chip select. 
Returns 0 for success.
