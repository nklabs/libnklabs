// Various test commands

#include <string.h>
#include "nksched.h"
#include "nkcli.h"

int test_tid;

void test_event(void *data)
{
    nk_printf("Ping!\n");
}

static int cmd_test(nkinfile_t *args)
{
    int val;
    if (!test_tid)
        test_tid = nk_alloc_tid();
    if (nk_fscan(args, "stop ")) {
        nk_printf("Sending Ctrl-S\n");
        nk_putc('S'-'@');
    } else if (nk_fscan(args, "wdt ")) {
        nk_printf("Entering infinite loop, WDT should reset MCU\n");
        for (;;) {
        }
    } else if (nk_fscan(args, "sched ")) {
        nk_printf("Scheduling a test event for 100 ms from now\n");
        nk_sched(test_tid, test_event, NULL, 100, "Test");
    } else if (nk_fscan(args, "sched %d ", &val)) {
        nk_printf("Scheduling a test event %d ms from now\n", val);
        nk_sched(test_tid, test_event, NULL, val, "Test");
    } else {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_test,
    ">test                      Test commands\n"
    "-test stop                 Emit ^S\n"
    "-test wdt                  Infinite loop to test WDT\n"
    "-test sched                Test scheduler event\n"
    "-test sched nn             Test scheduler in nn ms\n"
)
