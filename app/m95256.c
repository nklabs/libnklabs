#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "led.h"
#include "nkspi.h"
#include "nkspiflash.h"

extern uint8_t spi_buffer[];

// M95256 EEPROM on X-NUCLEO-EEPRMA2 board

const nkspi_device_t m95256_bus =
{
#ifdef NK_PLATFORM_STM32
    .cs_port = ARD_D9_GPIO_Port,
    .cs_pin = ARD_D9_Pin,
    .hspi = &ARD_SPI
#endif

#ifdef NK_PLATFORM_ATSAM
    .cs_pin = ARD_D9,
    .hspi = &ARD_SPI
#endif
};

const struct nk_spiflash_info m95256 =
{
    .spi_transfer = nk_spi_transfer,
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

COMMAND(cmd_m95256,
    ">m95256                    Read/write memory\n"
    "-m95256   rd <addr>        Read word\n"
    "-m95256   wr <addr> <val>  Write word\n"
    "-m95256   hd <addr> <len>  Hex dump memory\n"
    "-m95256   hd <addr>        Hex dump 256 bytes\n"
    "-m95256   hd               Hex dump next 256 bytes\n"
    "-m95256   crc <addr> <len> Calculate CRC of memory\n"
    "-m95256   erase <addr> <len>\n"
    "-                          Erase memory\n"
    "-m95256   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m95256   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
