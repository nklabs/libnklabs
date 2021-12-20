#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "led.h"
#include "nkspi.h"
#include "nkspiflash.h"

extern uint8_t spi_buffer[];

// M95M04 EEPROM on X-NUCLEO-EEPRMA2 board

const nkspi_device_t m95m04_bus =
{
#ifdef NK_PLATFORM_STM32
    .cs_port = ARD_D10_GPIO_Port,
    .cs_pin = ARD_D10_Pin,
    .hspi = &ARD_SPI
#endif

#ifdef NK_PLATFORM_ATSAM
    .cs_pin = ARD_D10,
    .hspi = &ARD_SPI
#endif
};

const struct nk_spiflash_info m95m04 =
{
    .spi_transfer = nk_spi_transfer,
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

COMMAND(cmd_m95m04,
    ">m95m04                    Read/write memory\n"
    "-m95m04   rd <addr>        Read word\n"
    "-m95m04   wr <addr> <val>  Write word\n"
    "-m95m04   hd <addr> <len>  Hex dump memory\n"
    "-m95m04   hd <addr>        Hex dump 256 bytes\n"
    "-m95m04   hd               Hex dump next 256 bytes\n"
    "-m95m04   crc <addr> <len> Calculate CRC of memory\n"
    "-m95m04   erase <addr> <len>\n"
    "-                          Erase memory\n"
    "-m95m04   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m95m04   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
