#include "nkcli.h"
#include "nksched.h"
#include "nkdriver_tm1638.h"
#include "pins.h"

const nk_tm1638_t tm1638 =
{
    .clk = &nk_pin_table[PIN_IDX_ARD_D2],
    .dio = &nk_pin_table[PIN_IDX_ARD_D3],
    .stb = &nk_pin_table[PIN_IDX_ARD_D4],
    .trise = 2,
    .tfall = 1,
    .digit_map = { 0, 1, 2, 3, 4, 5, 6, 7 } // Map so that digit[0] is left-mode
};

int tm1638_tid;
int tm1638_counter;

void tm1638_count(void *data)
{
    uint8_t buf[9];
    nk_snprintf((char *)buf, 9, "%6d--", tm1638_counter);
    ++tm1638_counter;
    nk_tm1638_display(&tm1638, 4, buf);
    nk_sched(tm1638_tid, tm1638_count, NULL, 1000, "TM1638 counter");
}

static int cmd_tm1638(nkinfile_t *args)
{
    int rtn;
    int bright;
    if (nk_fscan(args, "init ")) {
        rtn = nk_tm1638_init(&tm1638);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %d %e", &bright)) {
	uint8_t write_array[8];
	uint32_t write_len = 0;
        while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
            if (write_len != 7)
	        ++write_len;
        rtn = nk_tm1638_display(&tm1638, bright, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu bytes\n", write_len);
        }
    } else if (nk_fscan(args, "count ")) {
        rtn = nk_tm1638_init(&tm1638);
        if (rtn) {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!tm1638_tid)
            tm1638_tid = nk_alloc_tid();
        tm1638_counter = 0;
        tm1638_count(NULL);
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tm1638,
    ">tm1638                    TM1638 commands\n"
    "-tm1638 init               Setup GPIO\n"
    "-tm1638 w ff ff ..         Write bytes\n"
)
