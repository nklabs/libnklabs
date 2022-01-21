#include "nkcli.h"
#include "nkdriver_tm1637.h"
#include "pins.h"

const nk_tm1637_t tm1637 =
{
    .clk = &nk_pin_table[PIN_IDX_ARD_D2],
    .dio = &nk_pin_table[PIN_IDX_ARD_D3]
};

static int cmd_tm1637(nkinfile_t *args)
{
    int rtn;
    int bright;
    if (nk_fscan(args, "init ")) {
        rtn = nk_tm1637_init(&tm1637);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %d %e", &bright)) {
	uint8_t write_array[20];
	uint32_t write_len = 0;
        while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
	    ++write_len;
        rtn = nk_tm1637_display(&tm1637, bright, write_array, write_len);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu bytes\n", write_len);
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tm1637,
    ">tm1637                    TM1637 commands\n"
    "-tm1637 init               Setup GPIO\n"
    "-tm1637 w ff ff ..         Write bytes\n"
)
