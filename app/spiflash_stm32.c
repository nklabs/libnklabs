#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "led.h"
#include "nkspiflash.h"

#include "main.h" // For pin definitions

// How to access a particular SPI-flash device

struct flash_device
{
        GPIO_TypeDef *cs_port; // Port of CS_L line
        uint16_t cs_pin; // Pin of CS_L line
        SPI_HandleTypeDef *hspi; // Spi handle
};

// SPI-transfer

static int flashio(void *spi_ptr, uint8_t *data, uint32_t len)
{
        struct flash_device *info = (struct flash_device *)spi_ptr;

        shared_gpio_setup_for_spi(); // For STM32 boards...

        HAL_GPIO_WritePin(info->cs_port, info->cs_pin, 0); // CS_L

	HAL_SPI_TransmitReceive(info->hspi, data, data, len, 1000);

        HAL_GPIO_WritePin(info->cs_port, info->cs_pin, 1); // CS_L

        shared_gpio_setup_for_led(); // For STM32 boards...

        return 0;
}

// Transfer buffer

uint8_t spi_buffer[132];

// M95M04 EEPROM on X-NUCLEO-EEPRMA2 board

const struct flash_device m95m04_bus =
{
    .cs_port = MAIN_SPI_CS_L_GPIO_Port,
    .cs_pin = MAIN_SPI_CS_L_Pin,
    .hspi = &MAIN_SPI
};

const struct nk_spiflash_info m95m04 =
{
	.spi_transfer = flashio,
	.spi_ptr = (void *)&m95m04_bus,
	.buffer = spi_buffer,
	.page_size = 128,
	.busy_timeout = 10000,
	.addr_size = 3,
	.n_erase_options = 0
};

uint32_t m95m04_old_addr;

static int cmd_m95m04(nkinfile_t *args)
{
    return nk_spiflash_command(&m95m04, args, &m95m04_old_addr);
}

COMMAND(m95m04,
    "m95m04                    Read/write memory\n",
    "m95m04   rd <addr>        Read word\n"
    "m95m04   wr <addr> <val>  Write word\n"
    "m95m04   hd <addr> <len>  Hex dump memory\n"
    "m95m04   hd <addr>        Hex dump 256 bytes\n"
    "m95m04   hd               Hex dump next 256 bytes\n"
    "m95m04   crc <addr> <len> Calculate CRC of memory\n"
    "m95m04   erase <addr> <len>\n"
    "                          Erase memory\n"
    "m95m04   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m95m04   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)

// M95256 EEPROM on X-NUCLEO-EEPRMA2 board

const struct flash_device m95256_bus =
{
    .cs_port = MAIN_SPI_CS_L_1_GPIO_Port,
    .cs_pin = MAIN_SPI_CS_L_1_Pin,
    .hspi = &MAIN_SPI
};

const struct nk_spiflash_info m95256 =
{
	.spi_transfer = flashio,
	.spi_ptr = (void *)&m95256_bus,
	.buffer = spi_buffer,
	.page_size = 64,
	.busy_timeout = 10000,
	.addr_size = 2,
	.n_erase_options = 0
};

uint32_t m95256_old_addr;

static int cmd_m95256(nkinfile_t *args)
{
    return nk_spiflash_command(&m95256, args, &m95256_old_addr);
}

COMMAND(m95256,
    "m95256                    Read/write memory\n",
    "m95256   rd <addr>        Read word\n"
    "m95256   wr <addr> <val>  Write word\n"
    "m95256   hd <addr> <len>  Hex dump memory\n"
    "m95256   hd <addr>        Hex dump 256 bytes\n"
    "m95256   hd               Hex dump next 256 bytes\n"
    "m95256   crc <addr> <len> Calculate CRC of memory\n"
    "m95256   erase <addr> <len>\n"
    "                          Erase memory\n"
    "m95256   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m95256   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)

// M95040 EEPROM on X-NUCLEO-EEPRMA2 board

const struct flash_device m95040_bus =
{
    .cs_port = MAIN_SPI_CS_L_2_GPIO_Port,
    .cs_pin = MAIN_SPI_CS_L_2_Pin,
    .hspi = &MAIN_SPI
};

const struct nk_spiflash_info m95040 =
{
	.spi_transfer = flashio,
	.spi_ptr = (void *)&m95040_bus,
	.buffer = spi_buffer,
	.page_size = 16,
	.busy_timeout = 1000000,
	.addr_size = 1,
	.n_erase_options = 0
};

uint32_t m95040_old_addr;

static int cmd_m95040(nkinfile_t *args)
{
    return nk_spiflash_command(&m95040, args, &m95040_old_addr);
}

COMMAND(m95040,
    "m95040                    Read/write memory\n",
    "m95040   rd <addr>        Read word\n"
    "m95040   wr <addr> <val>  Write word\n"
    "m95040   hd <addr> <len>  Hex dump memory\n"
    "m95040   hd <addr>        Hex dump 256 bytes\n"
    "m95040   hd               Hex dump next 256 bytes\n"
    "m95040   crc <addr> <len> Calculate CRC of memory\n"
    "m95040   erase <addr> <len>\n"
    "                          Erase memory\n"
    "m95040   fill <addr> <len>\n"
    "                          Fill memory with pattern\n"
    "m95040   fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)

void spiflash_init()
{
    // Nothing to do for STM32
}
