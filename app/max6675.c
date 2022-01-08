#include "nkcli.h"
#include "nkspi.h"

const nkspi_device_t max6675_bus =
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

int nk_max6675_read_temp(float *val)
{
    uint8_t buf[2];
    uint16_t v;
    int rtn = nk_spi_transfer(&max6675_bus, buf, 2);
    v = (buf[0] << 8) + buf[1];
    *val = (float)(v >> 3) * .25f;
    return rtn;
}

static int cmd_max6675(nkinfile_t *args)
{
    if (nk_fscan(args, "")) {
        float val;
        int rtn = nk_max6675_read_temp(&val);
        if (rtn) {
            nk_printf("Couldn't access MAX6675\n");
        } else {
            nk_printf("Temp = %g C (%g F)\n", val, val*1.8f+32.0f);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_max6675,
    ">max6675                   MAX6675 commands\n"
    "-max6675                   Read temperature\n"
)
