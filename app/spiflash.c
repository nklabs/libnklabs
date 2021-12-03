#include <string.h>
#include "nkarch.h"
#include "nkcli.h"
#include "nkspiflash.h"

#include "main.h" // For pin definitions

// SPI-transfer

static int flashio(void *spi_ptr, uint8_t *data, uint32_t len)
{
        HAL_GPIO_WritePin(SPI1_CS_L_GPIO_Port, SPI1_CS_L_Pin, 0); // CS_L

	HAL_SPI_TransmitReceive(&spi1_port, data, data, len, 1000);

        HAL_GPIO_WritePin(SPI1_CS_L_GPIO_Port, SPI1_CS_L_Pin, 1); // CS_L

        return 0;
}

uint8_t spi_buffer[132];

struct nk_spiflash_info myflash =
{
	.spi_transfer = flashio,
	.spi_ptr = NULL,
	.buffer = spi_buffer,
	.page_size = 128,
	.busy_timeout = 10000,
	.addr_size = 3,
	.n_erase_options = 0
};

uint32_t my_old_addr;

static int cmd_spiflash(nkinfile_t *args)
{
    return nk_spiflash_command(&myflash, args, &my_old_addr);
}

COMMAND(spiflash,
    "spiflash                  Read/write flash memory\n",
    "spiflash rd <addr>        Read word\n"
    "spiflash wr <addr> <val>  Write word\n"
    "spiflash hd <addr> <len>  Hex dump flash memory\n"
    "spiflash crc <addr> <len> Calculate CRC of flash memory\n"
    "spiflash erase <addr> <len>\n"
    "                          Erase flash memory\n"
    "spiflash fill <addr> <len>\n"
    "                          Write flash memory\n",
    ""
)
