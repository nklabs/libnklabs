// Copyright 2021 NK Labs, LLC

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

// Abstract PWM pins

// TODO: everything

#ifdef PWM_ENABLED

#include <stdbool.h>
#include "nkarch.h"

#ifndef _Inkpin
#define _Inkpin

typedef struct nk_pin nk_pin_t;
typedef struct nk_pin_func nk_pin_func_t;

// Pin modes

typedef enum {
    NK_PINMODE_INPUT,
    NK_PINMODE_INPUT_PULLUP,
    NK_PINMODE_OUTPUT,
    NK_PINMODE_OUTPUT_OD,
    NK_PINMODE_OUTPUT_OD_PULLUP,
    NK_PINMODE_COUNT
} nk_pinmode_t;

struct gpio_mode_entry {
    const char *name;
    int mode;
    int pull;
    int speed;
};

extern const struct gpio_mode_entry nk_pinmode_table[];

// A pin

struct nk_pin {
    const nk_pin_func_t *func;
    const char *name;

#ifdef NK_PLATFORM_STM32
    GPIO_TypeDef *port; // Port of CS_L line
    uint16_t pin; // Pin of CS_L line
#endif

#ifdef NK_PLATFORM_ATSAM
    uint8_t pin; // Pin of CS_L line
#endif
};

// Pin functions

struct nk_pin_func {
    int (*pin_read)(const nk_pin_t *pin, bool *val);
    int (*pin_write)(const nk_pin_t *pin, bool val);
    int (*pin_setmode)(const nk_pin_t *pin, nk_pinmode_t mode);
    int (*pin_getmode)(const nk_pin_t *pin, nk_pinmode_t *pinmode);
};

extern const nk_pin_func_t nk_pin_funcs;

// User provides this table

extern const nk_pin_t nk_pin_table[];

// User functions

// Read pin
#define nk_pin_read(pin, val) (pin)->func->pin_read((pin), (val))

// Write pin
#define nk_pin_write(pin, val) (pin)->func->pin_write((pin), (val))

// Set up pin for a specific mode
#define nk_pin_setmode(pin, mode) (pin)->func->pin_setmode((pin), (mode))

// Get current mode
#define nk_pin_getmmode(pin) (pin)->func->pin_getmode(pin)

#endif

#endif