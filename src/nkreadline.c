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

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "nkuart.h"
#include "nkcli.h"
#include "nkreadline.h"

// Console line editor buffer

int console_tid;
void (*console_callback)(char *s);
const char *console_prompt;

static char tty_buf[NKREADLINE_LINE_SIZE];
static size_t tty_len;
static size_t tty_cur;
static bool changed; // Set if buffer has been edited

// History buffer

static char histbuf[NKREADLINE_HIST_SIZE]; // NUL terminated lines
static size_t hist_end; // Number of bytes in history buffer
static size_t hist_idx; // Index to start of current history line

static bool echo_mode = 1;

static uint8_t mode;
static uint8_t tab_count;

bool nk_set_echo(bool new_mode)
{
        bool org = echo_mode;
        echo_mode = new_mode;
        return org;
}

bool nk_get_echo()
{
        return echo_mode;
}

static void console_putchar(char c)
{
        if (echo_mode)
                nk_putc(c);
}

static void console_puts(const char *s)
{
	if (echo_mode)
		nk_puts(s);
}

// Move history pointer to previous line
// Return address of this previous line or NULL if there is none
static char *hist_prev(void)
{
	if (hist_idx == 0)
		return NULL;
	do {
		--hist_idx;
	} while (hist_idx && histbuf[hist_idx - 1]);
	return histbuf + hist_idx;
}

// Move history pointer to next line
// Return address of this next line or NULL if there is none
static char *hist_next(void)
{
	if (hist_idx == hist_end)
		return NULL;
	while (histbuf[hist_idx])
		++hist_idx;
	++hist_idx;
	if (hist_idx == hist_end)
		return NULL;
	else
		return histbuf + hist_idx;
}

// Delete current line from history buffer
// Returns number of bytes deleted
static size_t hist_del()
{
	if (hist_idx != hist_end) {
		size_t len = strlen(histbuf + hist_idx) + 1;
		if (hist_end - (hist_idx + len))
			memmove(histbuf + hist_idx, histbuf + hist_idx + len, hist_end - (hist_idx + len));
		hist_end -= len;
		return len;
	} else {
		return 0;
	}
}

// Insert line into history buffer
static void hist_ins(char *buf)
{
	size_t len = strlen(buf) + 1;
	size_t tmp = hist_idx;
	// Delete oldest history lines in case we are out of space
	hist_idx = 0;
	while (len + hist_end > NKREADLINE_HIST_SIZE) {
		size_t r = hist_del();
		if (tmp)
			tmp -= r;
	}
	hist_idx = tmp;
	// Make space for this new line
	if (hist_idx != hist_end) {
		memmove(histbuf + hist_idx + len, histbuf + hist_idx, hist_end - hist_idx);
	}
	memcpy(histbuf + hist_idx, buf, len);
	hist_end += len;
}

// Move cursor to end of line
static void to_end()
{
	while (tty_cur != tty_len) {
		console_putchar(tty_buf[tty_cur++]);
	}
}

// Move cursor to beginning of line
static void to_beg()
{
	while (tty_cur) {
		console_putchar(8);
		--tty_cur;
	}
}

// Erase screen line, clear edit buffer
static void erase()
{
	size_t x;
	to_end();
	for (x = 0; x != tty_len; ++x) {
		console_putchar(8), console_putchar(' '), console_putchar(8);
	}
	tty_cur = 0;
	tty_len = 0;
}

// Redraw line, put cursor at end
static void redraw()
{
	tty_cur = 0;
	console_puts(console_prompt);
	to_end();
}

// Type screen line
static void type()
{
	tty_cur = 0;
	to_end();
}

// Handle up arrow: move previous history line into edit buffer
static void hist_up()
{
	if (hist_idx) {
		// Insert possibly modified line
		if (tty_len) {
			if (!changed) {
				erase();
			} else {
				tty_buf[tty_len] = 0;
				hist_ins(tty_buf);
				erase();
			}
		}
		// Get previous history line into edit buffer
		if (hist_prev()) {
			strcpy(tty_buf, histbuf + hist_idx);
			tty_len = strlen(tty_buf);
			type();
		}
		changed = 0;
	} else {
		console_putchar(7);
	}
}

// Handle down arrow: move next history line into edit buffer
static void hist_down()
{
	if (hist_idx != hist_end &&  hist_idx + strlen(histbuf + hist_idx) + 1 != hist_end) {
		// We have more history
		if (tty_len) {
			if (changed) {
				hist_next();
				tty_buf[tty_len] = 0;
				hist_ins(tty_buf);
			}
			hist_next();
			erase();
		}
		if (hist_idx != hist_end) {
			strcpy(tty_buf, histbuf + hist_idx);
			tty_len = strlen(tty_buf);
			type();
		}
		changed = 0;
	} else if (tty_len) {
		// No more history
		if (changed) {
			hist_next();
			tty_buf[tty_len] = 0;
			hist_ins(tty_buf);
		}
		hist_idx = hist_end;
		erase();
		changed = 0;
	} else {
		console_putchar(7);
	}
}

static void cur_left()
{
	if (tty_cur) {
		console_putchar(8);
		--tty_cur;
	}
}

static void cur_right()
{
	if (tty_cur != tty_len) {
		console_putchar(tty_buf[tty_cur++]);
	}
}

static void typech(char c)
{
	size_t n;
	if (tty_len != NKREADLINE_LINE_SIZE - 1) {
		if (tty_cur != tty_len) {
			memmove(tty_buf + tty_cur + 1, tty_buf + tty_cur, tty_len - tty_cur);
		}
		++tty_len;
		tty_buf[tty_cur] = c;
		for (n = tty_cur; n != tty_len; ++n)
			console_putchar(tty_buf[n]);
		tty_cur++;
		while (n != tty_cur) {
			console_putchar(8);
			--n;
		}
		changed = 1;
	} else {
		console_putchar(7); // Beep!
	}
}

static void delch()
{
	if (tty_cur != tty_len) {
		size_t n;
		if (tty_cur + 1 != tty_len) {
			memmove(tty_buf + tty_cur, tty_buf + tty_cur + 1, tty_len - (tty_cur + 1));
		}
		--tty_len;
		for (n = tty_cur; n != tty_len; ++n)
			console_putchar(tty_buf[n]);
		console_putchar(' ');
		console_putchar(8);
		for (n = tty_cur; n != tty_len; ++n)
			console_putchar(8);
		changed = 1;
	}
}

static void prompt_task(void *data)
{
	int ch;
	(void)data;
	for (;;) {
		ch = nk_getc();
		if (ch == -1) {
			// No more data
			// HAL_UART_Receive_IT(&console_uart, &tty_ch, 1);
			// Polling because interrupts occur only after four bytes
			nk_set_uart_callback(console_tid, prompt_task, NULL);
			break;
		}
		if (tab_count)
			--tab_count;
		if (mode == 0) {
			if (ch >= 32 && ch <= 126) {
				typech((char)ch);
			} else if (ch == 9) { // TAB completion
				if (tty_len && tty_buf[tty_len - 1] == ' ') {
					console_putchar('\n');
					tty_buf[tty_len - 1] = 0;
					if (nk_help(tty_buf)) {
						console_putchar(7);
					}
					tty_buf[tty_len - 1] = ' ';
					redraw();
				} else {
					for (;;) {
						tty_buf[tty_len] = 0;
						if (tab_count) {
							console_putchar('\n');
						}
						// Get next character
						ch = nk_complete(tty_buf, tab_count);
						if (tab_count) {
							redraw();
						}
						if (ch == -1) { // No matches
							console_putchar(7);
							break;
						} else if (ch == -2) { // Multiple matches
							if (!tab_count)
								console_putchar(7);
							tab_count = 2;
							break;
						} else {
							typech((char)ch);
							if (ch == ' ')
								break;
						}
					}
				}
			} else if ((ch == 8 || ch == 127)) { // ^H, DEL
				if (tty_cur) {
					cur_left();
					delch();
				} else {
					console_putchar(7);
				}
			} else if (ch == 4) { // ^D
				if (tty_cur != tty_len) {
					delch();
				} else {
					console_putchar(7);
				}
			} else if (ch == 3) { // ^C
				tty_buf[tty_len] = 0;
				if (changed) {
					if (tty_len) {
						hist_ins(tty_buf);
					}
				}
				hist_idx = hist_end;
				to_end();
				nk_putc('^');
				nk_putc('C');
				nk_putc('\n');
				nk_puts(console_prompt); // Reprint prompt
				tty_len = 0;
				tty_cur = 0;
				changed = 0;
			} else if (ch == 5) { // ^E
				to_end();
			} else if (ch == 1) { // ^A
				to_beg();
			} else if (ch == 13 || ch == 10) {
				to_end();
				tty_buf[tty_len] = 0;
				if (tty_len) {
					if (!changed) {
						// Delete orginal so that we don't get tons of history
						// when you repeat the same command repeatedly
						// This has effect of moving retrieved line to end of history
						hist_del();
					}
					hist_idx = hist_end;
					hist_ins(tty_buf);
				}
				hist_idx = hist_end;
				nk_putc('\n');
				changed = 0;
				console_callback(tty_buf);
				break;
			} else if (ch == 16) { // ^P
				hist_up();
			} else if (ch == 14) { // ^N
				hist_down();
			} else if (ch == 2) { // ^B
				cur_left();
			} else if (ch == 6) { // ^F
				cur_right();
			} else if (ch == 27) {
				mode = 1;
			} else {
				// Unknown char, ignore it
			}
		} else if (mode == 1) { // ESC key pressed
			if (ch == '[') {
				mode = 2;
			} else {
				mode = 0;
			}
		} else if (mode == 2) { // ESC [ pressed
			if (ch == 'A') { // Up arrow
				hist_up();
			} else if (ch == 'B') { // Down arrow
				hist_down();
			} else if (ch == 'C') { // Right arrow
				cur_right();
			} else if (ch == 'D') { // Left arrow
				cur_left();
			}
			mode = 0;
		}
	}
}

void nk_readline(int tid, void (*f)(char *line), const char *prompt_text)
{
	tty_cur = 0;
	tty_len = 0;
	changed = 0;
	console_prompt = prompt_text;
	console_callback = f;
	console_tid = tid;
	console_puts(console_prompt);
	nk_set_uart_callback(console_tid, prompt_task, NULL);
}
