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

#ifndef _Inkcli
#define _Inkcli

#include "nkmacros.h"
#include "nkscan.h"
#include "nkprintf.h"

#define CMD_FLAG_HIDDEN 1 // Command is hidden unless in "factory service mode"

struct console_cmd {
    const char *text; // Command name and help
    int (*func)(nkinfile_t *args); // Command function
};

// Macros to help declare commands
// There is a section dedicated to the command table.  These macros add entries to this table.
// This way we avoid having a single file with a giant command table.  Instead the command registration and help can be placed right next to the command function

// __attribute__((used)) prevents compiler from optimizing unreferenced data out
// __attribute__((unused)) prevents compiler from complaining about unused variables

#define COMMAND(func, text) \
static struct console_cmd cmd_entry_ ## func __attribute__ ((section (".COMMAND_TABLE"))) __attribute__((unused)) __attribute__((used)) = \
  { (text), (func) };

// Start up CLI, print first prompt
// (this submits a task, so prompt is issued on next return to sched)
void nk_init_cli(void);

extern int facmode; // Set if we are in factory mode

#endif
