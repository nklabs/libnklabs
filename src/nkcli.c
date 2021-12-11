// Copyright 2020 NK Labs, LLC

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to permit
// persons to whom the Software is furnished to do so, subject to the
// following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <string.h>
#include "nksched.h"
#include "nkreadline.h"
#include "nkcli.h"

int facmode = 1; // Set if we are in factory mode

// Command table

static int cmd_help(nkinfile_t *args);

COMMAND(help,
    "help                      Help command\n",
    "help <name>               Print help for a command\n"
    "help                      List available commands\n"
    "help all                  Print help for all commands\n",
    ""
)

extern unsigned char __start_COMMAND_TABLE;
extern unsigned char __stop_COMMAND_TABLE;

// Find a command in the command table: return index to it or -1 if not found

static struct console_cmd *find_cmd(char *s)
{
    struct console_cmd *x;
    for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
        if ((facmode || !(x->flags & CMD_FLAG_HIDDEN)) && !strcmp(x->name, s)) {
            return x;
        }
    }
    return 0;
}

// Print detailed help for a specific command by index

static void specific_help(struct console_cmd *x)
{
    nk_puts(x->main_help);
    if (facmode && x->hidden_help)
        nk_puts(x->hidden_help);
}

// Help command

static int cmd_help(nkinfile_t *args)
{
    char buf[80];
    struct console_cmd *x;
    if (nk_fscan(args, "all ")) {
        int first = 0;
        for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
            if (facmode || !(x->flags & CMD_FLAG_HIDDEN)) {
                if (first)
                    nk_putc('\n');
                nk_puts(x->short_help);
                specific_help(x);
                first = 1;
            }
        }
    } else if (nk_fscan(args, "%w ", buf, sizeof(buf))) {
        x = find_cmd(buf);
        if (x) {
            specific_help(x);
        } else
            nk_puts("Unknown command\n");
    } else if (nk_fscan(args, "")) {
        nk_printf("help <name> for help with a specific command\n\n");
        nk_printf("Available commands:\n\n");
        for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
            // One line of help per command
            if (facmode || !(x->flags & CMD_FLAG_HIDDEN))
                nk_puts(x->short_help);
        }
    } else {
        nk_puts("Syntax error\n");
    }
    return 0;
}

static void process_cmd(char *pCmd)
{
    int	rtn = 0;
    char cmd[80];
    nkinfile_t f[1];
    nkinfile_open_string(f, pCmd);

    if (nk_feof(f) || nk_fpeek(f) == '*') // Ignore empty commands and Zmodem junk
        return;

    memset(cmd, 0, sizeof(cmd));

    if (nk_fscan(f, "%w %e", cmd, sizeof(cmd))) {
        struct console_cmd *x = find_cmd(cmd);
        if (!x) {
            nk_printf("Syntax error\n");
            return;
        } else if (nk_fscan(f, "help ")) {
            specific_help(x);
        } else {
            rtn = x->func(f);
            if (rtn) {
                nk_printf("Command failed!\n");
            }
        }
    }
}

int cli_tid;

static void cmd_next(char *s)
{
	process_cmd(s);
	nk_readline(cli_tid, cmd_next, ">");
}

static void cli_first(void *p)
{
	(void)p;
	cmd_next("");
}

void nk_init_cli()
{
	startup("Command Line Interface\n");
	cli_tid = nk_alloc_tid();
	nk_sched(cli_tid, cli_first, NULL, 0, "CLI start");
}
