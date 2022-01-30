#include "nkcli.h"
#include "nksched.h"
#include "nkdriver_tm1637.h"
#include "pins.h"

const nk_tm1637_t tm1637 =
{
    .clk = &nk_pin_table[PIN_IDX_ARD_D2],
    .dio = &nk_pin_table[PIN_IDX_ARD_D3],
    // With .1 pF capacitors (some boards have these by mistake):
    .trise = 200,
    .tfall = 3,
    // With 100pF capacitors:
    //.trise = 2,
    //.tfall = 1,
    .digit_map = { 2, 1, 0, 5, 4, 3 } // Map so that digit[0] is left-mode
};

int tm1637_tid;
int tm1637_counter;

void tm1637_count(void *data)
{
    uint8_t buf[7];
    nk_snprintf((char *)buf, 7, "%4d--", tm1637_counter);
    ++tm1637_counter;
    nk_tm1637_display(&tm1637, 7, buf);
    nk_sched(tm1637_tid, tm1637_count, NULL, 1000, "TM1637 counter");
}

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
	uint8_t write_array[6];
	uint32_t write_len = 0;
        while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
            if (write_len != 5)
	        ++write_len;
        rtn = nk_tm1637_display(&tm1637, bright, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu bytes\n", write_len);
        }
    } else if (nk_fscan(args, "count ")) {
        rtn = nk_tm1637_init(&tm1637);
        if (rtn) {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!tm1637_tid)
            tm1637_tid = nk_alloc_tid();
        tm1637_counter = 0;
        tm1637_count(NULL);
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
