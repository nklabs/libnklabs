#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "led.h"
#include "nkspi.h"
#include "nkspiflash.h"

extern uint8_t spi_buffer[];

// M95040 EEPROM on X-NUCLEO-EEPRMA2 board

const nkspi_device_t m95040_bus =
{
#ifdef NK_PLATFORM_STM32
    .cs_port = ARD_D2_GPIO_Port,
    .cs_pin = ARD_D2_Pin,
    .hspi = &ARD_SPI
#endif

#ifdef NK_PLATFORM_ATSAM
    .cs_pin = ARD_D2,
    .hspi = &ARD_SPI
#endif
};

const struct nk_spiflash_info m95040 =
{
    .spi_transfer = nk_spi_transfer,
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

COMMAND(cmd_m95040,
    ">m95040                    Read/write memory\n"
    "-m95040   rd <addr>        Read word\n"
    "-m95040   wr <addr> <val>  Write word\n"
    "-m95040   hd <addr> <len>  Hex dump memory\n"
    "-m95040   hd <addr>        Hex dump 256 bytes\n"
    "-m95040   hd               Hex dump next 256 bytes\n"
    "-m95040   crc <addr> <len> Calculate CRC of memory\n"
    "-m95040   erase <addr> <len>\n"
    "-                          Erase memory\n"
    "-m95040   fill <addr> <len>\n"
    "-                          Fill memory with pattern\n"
    "-m95040   fill <addr> <len> <val>\n"
    "-                          Fill memory with constant\n"
)
