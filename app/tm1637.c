#include "nkcli.h"
#include "nksched.h"
#include "nkdriver_tm1637.h"
#include "pins.h"

const nk_tm1637_t tm1637 =
{
    .clk = &nk_pin_table[PIN_IDX_ARD_D2],
    .dio = &nk_pin_table[PIN_IDX_ARD_D3],

    // With .1 uF capacitors (some boards have these by mistake):
    // (Robotdyn displays)
    .trise = 200,
    .tfall = 3,

    // With 100pF capacitors:
    //.trise = 2,
    //.tfall = 1,

    // Map so that digit[0] is left-most

    .digit_map = { 0, 1, 2, 3, 4, 5 } // Normal
    //.digit_map = { 2, 1, 0, 5, 4, 3 } // RobotDyn 6-digit
};

int tm1637_tid;
int tm1637_counter;

void tm1637_count(void *data)
{
    uint8_t buf[7];
    nk_snprintf((char *)buf, 7, "%4d--", tm1637_counter);
    nk_tm1637_display(&tm1637, (1 << (tm1637_counter & 7)), buf);
    ++tm1637_counter;
    nk_sched(tm1637_tid, tm1637_count, NULL, 1000, "TM1637 counter");
}

static int cmd_tm1637(nkinfile_t *args)
{
    int rtn;
    int bright;
    if (nk_fscan(args, "init ")) {
        rtn = nk_tm1637_init(&tm1637);
        rtn |= nk_tm1637_display_on(&tm1637, 4);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if (nk_fscan(args, "w %e")) {
	uint8_t write_array[6];
	uint32_t write_len = 0;
        while (nk_fscan(args, "%hhx %e", &write_array[write_len]))
            if (write_len != 5)
	        ++write_len;
        rtn = nk_tm1637_display(&tm1637, 0, write_array);
        if (rtn) {
            nk_printf("No ack\n");
        } else {
            nk_printf("Wrote %lu bytes\n", write_len + 1);
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
    } else if(nk_fscan(args, "scan ")) {
        uint8_t val;
        rtn = nk_tm1637_keyscan(&tm1637, &val);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("%2.2x\n", val);
        }
    } else if(nk_fscan(args, "on %d ", &bright)) {
        rtn = nk_tm1637_display_on(&tm1637, bright);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else if(nk_fscan(args, "off ")) {
        rtn = nk_tm1637_display_off(&tm1637);
        if (rtn) {
            nk_printf("Pin access error\n");
        }
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_tm1637,
    ">tm1637                    TM1637 commands\n"
    "-tm1637 init               Setup GPIO\n"
    "-tm1637 on <brightness>    Display on with brightness 0 - 7\n"
    "-tm1637 off                Display off\n"
    "-tm1637 scan               Keyscan\n"
    "-tm1637 w ff ff ..         Write bytes\n"
    "-tm1637 count              Start counter\n"
)
