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

COMMAND(cmd_help,
    ">help                      Help command\n"
    "-help <name>               Print help for a command\n"
    "-help                      List available commands\n"
    "-help all                  Print help for all commands\n"
)

extern unsigned char __start_COMMAND_TABLE;
extern unsigned char __stop_COMMAND_TABLE;

static int name_match(const char *table, const char *cmd, int partial, int allow_hidden)
{
    if (table[0] == '>' || allow_hidden)
    {
        int x;
        ++table;
        for (x = 0; partial > 0 && table[x] && table[x] != ' ' && cmd[x]; ++x)
        {
            if (table[x] != cmd[x])
                break;
            --partial;
        }
        if (!partial)
            return 1;
        if (table[x] == ' ' && cmd[x] == 0)
            return 1;
    }
    return 0;
}

// Find a command in the command table: return index to it or -1 if not found

static struct console_cmd *find_cmd(char *s)
{
    struct console_cmd *x;
    for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
        if (name_match(x->text, s, 100, facmode))
            return x;
    }
    return 0;
}

// list_flag = 0: find next character to complete command, 0 if command already complete or -1 if there are none or multiple matches
// list_flag = 1: show all possible completions

int nk_complete(const char *s, int list_flag)
{
    int match_count = 0;
    struct console_cmd *match = 0;
    struct console_cmd *x;
    size_t len = strlen(s);
    
    for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
        if (name_match(x->text, s, len, facmode)) {
            match = x;
            ++match_count;
            if (list_flag)
            {
                int y;
                for (y = 1; x->text[y] && x->text[y] != ' '; ++y)
                    nk_putc(x->text[y]);
                nk_putc('\n');
            }
        }
    }
    if (match_count == 1) {
        return match->text[1 + len];
    } else if (match_count == 0) {
        return -1;
    } else {
        return -2;
    }
}

static const char *print_help(const char *s)
{
    ++s;
    while (*s && (*s != '\n'))
        nk_putc(*s++);
    if (*s)
        nk_putc(*s++);
    return s;
}

static const char *skip_help(const char *s)
{
    ++s;
    while (*s && (*s != '\n'))
        s++;
    if (*s)
        s++;
    return s;
}

// Print detailed help for a specific command by index

static void specific_help(const char *t)
{
    while (*t) {
        if (*t == '-' || (*t == '!' && facmode))
        {
            // We found a help line, print it
            t = print_help(t);
        }
        else
        {
            // Some other kind of line, skip it
            t = skip_help(t);
        }
    }
}

// Help command

static int cmd_help(nkinfile_t *args)
{
    char buf[80];
    struct console_cmd *x;
    if (nk_fscan(args, "all ")) {
        int first = 0;
        for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
            if (facmode || x->text[0] == '>') {
                if (first)
                    nk_putc('\n');
                specific_help(print_help(x->text));
                first = 1;
            }
        }
    } else if (nk_fscan(args, "%w ", buf, sizeof(buf))) {
        x = find_cmd(buf);
        if (x) {
            specific_help(x->text);
        } else
            nk_puts("Unknown command\n");
    } else if (nk_fscan(args, "")) {
        nk_printf("help <name> for help with a specific command\n\n");
        nk_printf("Available commands:\n\n");
        for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x != (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
            // One line of help per command
            if (facmode || x->text[0] == '>')
            {
                print_help(x->text);
            }
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
            specific_help(x->text);
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

int cmd_compare(const void *a, const void *b)
{
    const struct console_cmd *x = (struct console_cmd *)a;
    const struct console_cmd *y = (struct console_cmd *)b;
    return strcmp(x->text, y->text);
}

void nk_init_cli()
{
	nk_startup_message("Command Line Interface\n");

	// In theory this could be done by modifying the .elf file after
	// compiling- then command table would not use RAM.  But then we
	// have to write a special tool..
#if 0
	// Sort command table
        qsort(
            &__start_COMMAND_TABLE,
            (&__stop_COMMAND_TABLE - &__start_COMMAND_TABLE) / sizeof(struct console_cmd),
            sizeof(struct console_cmd),
            cmd_compare
        );
#else
        // Use bubble sort to save space
        int work;
        do {
            struct console_cmd *x;
            work = 0;
            for (x = (struct console_cmd *)&__start_COMMAND_TABLE; x + 1 < (struct console_cmd *)&__stop_COMMAND_TABLE; ++x) {
                if (strcmp(x->text, (x + 1)->text) > 0) {
                    struct console_cmd tmp = *x;
                    *x = *(x + 1);
                    *(x + 1) = tmp;
                    work = 1;
                }
            }
        } while (work);
#endif
	cli_tid = nk_alloc_tid();
	nk_sched(cli_tid, cli_first, NULL, 0, "CLI start");
}
