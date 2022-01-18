#include "nkcli.h"
#include "nkdriver_max6675.h"

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

static int cmd_max6675(nkinfile_t *args)
{
    if (nk_fscan(args, "")) {
        float val;
        int rtn = nk_max6675_read_temp(&max6675_bus, &val);
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
