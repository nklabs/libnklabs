# nkreadline: Command line editor

## Files

[nkreadline.c](../src/nkreadline.c), [nkreadline.h](../inc/nkreadline.h),
[nkreadline_config.h](../config/nkreadline_config.h)

nkreadline_config.h parameters:

Set input line buffer size and history buffer size (both in bytes).  These
determine the size of statically allocated arrays.

```c
#define NK_LINE_SIZE 256
#define NK_HIST_SIZE 4096
```

The nkreadline module provides a command line editor with history.  Malloc
is not used.

nk_readline calls the following functions from the UART module:

	nk_putc(), nk_puts(), nk_getc(), nk_set_uart_callback()

## nk_readline: prompt for a line of input

```c
void nk_readline(void (*f)(char *line), const char *prompt_text);
```

Prompt for a line of input text.  When the user hits Carriage Return
(Enter), the callback function 'f' is called with the line of text.  This is
a pointer to a statically allocated buffer.

Emacs-like command line editing is provided.  Shell-like history is
provided.

nkreadline has its own task ID.  When the callback is called, the current
task ID will be the nkreadline one.

## nk_set_echo, nk_get_echo: control echo flag

```c
int nk_set_echo(int flag);
```

Set echo mode.  If flag is true (non-zero), echoing is enabled.  If flag is
false (zero), echoing is disabled.  Return previous state of echo enable
flag.

```c
int nk_get_echo();
```

Get state of echo enable flag.
