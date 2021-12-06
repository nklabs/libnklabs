#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nkspiflash.h"

// How to access a particular SPI-flash device

struct flash_device
{
    uint8_t cs_pin; // Pin of CS_L line
    struct spi_m_sync_descriptor *hspi; // SPI device handle
};


// SPI-transfer

static int flashio(void *spi_ptr, uint8_t *data, uint32_t len)
{
    int rtn;
    struct spi_xfer xfer =
    {
        .txbuf = data,
        .rxbuf = data,
        .size = len
    };
    struct flash_device *info = (struct flash_device *)spi_ptr;

    gpio_set_pin_level(info->cs_pin, false);

    rtn = spi_m_sync_transfer(info->hspi, &xfer);

    gpio_set_pin_level(info->cs_pin, true);

    nk_printf("transfer returned %d\n", rtn);

    return 0;
}

// Transfer buffer

uint8_t spi_buffer[132];

// AT25DF081A SPI-flash on X-NUCLEO-EEPRMA2 board

const struct flash_device at25df081_bus =
{
    .cs_pin = SPI_CS_L,
    .hspi = &SPI_0
};

const struct nk_spiflash_info at25df081 =
{
    .spi_transfer = flashio,
    .spi_ptr = (void *)&at25df081_bus,
    .buffer = spi_buffer,
    .page_size = 128,
    .busy_timeout = 10000,
    .addr_size = 3,
    .n_erase_options = 3,
    .erase_options = {
        { 65536, 0xD8 },
        { 32768, 0x52 },
        { 4096, 0x20 }
    }
};

uint32_t at25df081_old_addr;

static int cmd_at25df081(nkinfile_t *args)
{
    return nk_spiflash_command(&at25df081, args, &at25df081_old_addr);
}

COMMAND(at25df081,
    "at25df081                  Read/write memory\n",
    "at25df081 rd <addr>        Read word\n"
    "at25df081 wr <addr> <val>  Write word\n"
    "at25df081 hd <addr> <len>  Hex dump memory\n"
    "at25df081 hd <addr>        Hex dump 256 bytes\n"
    "at25df081 hd               Hex dump next 256 bytes\n"
    "at25df081 crc <addr> <len> Calculate CRC of memory\n"
    "at25df081 erase <addr> <len>\n"
    "                           Erase memory\n"
    "at25df081 fill <addr> <len>\n"
    "                           Fill memory with pattern\n"
    "at25df081 fill <addr> <len> <val>\n"
    "                          Fill memory with constant\n",
    ""
)

void spiflash_init()
{
    startup("SPI-Flash\n");
    spi_m_sync_enable(&SPI_0);
}
