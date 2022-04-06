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

// Abstract GPIO pins

// Each pin has:
//   A name string
//   Can be found in nk_pin_table (pins.c) via enumeration index (pins.h)
//   Can be accessed by address (it's the address of the pin table entry)
//   Can be accessed via virtual functions, so that pins not part of the MCU can also be accessed with a different set of functions

// A CLI is provided for accessing the pins on the command line

#include <string.h>
#include "nkcli.h"
#include "pins.h"

int hal_pin_read(const nk_pin_t *pin, bool *val)
{
#ifdef NK_PLATFORM_STM32
    *val = (GPIO_PIN_SET == HAL_GPIO_ReadPin(pin->port, pin->pin));
#endif

#ifdef NK_PLATFORM_ATSAM
    *val = (true == gpio_get_pin_level(pin->pin));
#endif
    return 0;
}

int hal_pin_write(const nk_pin_t *pin, bool val)
{
#ifdef NK_PLATFORM_STM32
    HAL_GPIO_WritePin(pin->port, pin->pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif

#ifdef NK_PLATFORM_ATSAM
    gpio_set_pin_level(pin->pin, val ? true : false);
#endif
    return 0;
}

int hal_pin_setmode(const nk_pin_t *pin, nk_pinmode_t mode)
{
#ifdef NK_PLATFORM_STM32
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin->pin;
    GPIO_InitStruct.Mode = nk_pinmode_table[mode].mode;
    GPIO_InitStruct.Pull = nk_pinmode_table[mode].pull;
    GPIO_InitStruct.Speed = nk_pinmode_table[mode].speed;
    //GPIO_InitStruct.Alternate = 0; // FIXME
    HAL_GPIO_Init(pin->port, &GPIO_InitStruct);
#endif

#ifdef NK_PLATFORM_ATSAM
    gpio_set_pin_direction(pin->pin, nk_pinmode_table[mode].mode);
    gpio_set_pin_pull_mode(pin->pin, nk_pinmode_table[mode].pull);
#endif
    return 0;
}

int hal_pin_getmode(const nk_pin_t *pin, nk_pinmode_t *pinmode)
{
    // HAL does not provide this..
    *pinmode = 0;
    return 0;
}

const nk_pin_func_t nk_pin_funcs =
{
    .pin_read = hal_pin_read,
    .pin_write = hal_pin_write,
    .pin_setmode = hal_pin_setmode,
    .pin_getmode = hal_pin_getmode
};


const struct gpio_mode_entry nk_pinmode_table[] =
{
#ifdef NK_PLATFORM_STM32
    [NK_PINMODE_INPUT] = { "input", GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM },
    [NK_PINMODE_INPUT_PULLUP] = { "input_pullup", GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM },
    [NK_PINMODE_OUTPUT] = { "output", GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM },
#endif

#ifdef NK_PLATFORM_ATSAM
    [NK_PINMODE_INPUT] = { "input", GPIO_DIRECTION_IN, GPIO_PULL_OFF, 0 },
    [NK_PINMODE_INPUT_PULLUP] = { "input_pullup", GPIO_DIRECTION_IN, GPIO_PULL_UP, 0 },
    [NK_PINMODE_OUTPUT] = { "output", GPIO_DIRECTION_OUT, GPIO_PULL_OFF, 0 },
#endif
};


int nk_pin_lookup(const char *s)
{
    int pin_idx;
    for (pin_idx = 0; pin_idx != PIN_COUNT; ++pin_idx)
    	if (!strcmp(nk_pin_table[pin_idx].name, s))
    		return pin_idx;
    return -1;
}

nk_pinmode_t nk_pin_mode_lookup(const char *s)
{
    int mode_idx;
    for (mode_idx = 0; mode_idx != NK_PINMODE_COUNT; ++mode_idx)
    	if (!strcmp(nk_pinmode_table[mode_idx].name, s))
    		break;
    return mode_idx;
}

int cmd_pin(nkinfile_t *args)
{
    int pin_idx;
    char pin_name[20];
    char pin_mode[20];
    if (nk_fscan(args, "")) {
    	for (pin_idx = 0; pin_idx != PIN_COUNT; ++pin_idx) {
	    bool val;
	    nk_pin_read(&nk_pin_table[pin_idx], &val);
	    nk_printf("%s = %d\n", nk_pin_table[pin_idx].name, val);
    	}
    } else if (nk_fscan(args, "set %w ", pin_name, sizeof(pin_name))) {
    	pin_idx = nk_pin_lookup(pin_name);
    	if (pin_idx < 0) {
	    nk_printf("Unknown pin\n");
    	} else {
	    nk_pin_write(&nk_pin_table[pin_idx], 1);
    	}
    } else if (nk_fscan(args, "clear %w ", pin_name, sizeof(pin_name))) {
    	pin_idx = nk_pin_lookup(pin_name);
    	if (pin_idx < 0) {
	    nk_printf("Unknown pin\n");
    	} else {
	    nk_pin_write(&nk_pin_table[pin_idx], 0);
    	}
    } else if (nk_fscan(args, "mode %w %w ", pin_name, sizeof(pin_name), pin_mode, sizeof(pin_mode))) {
    	pin_idx = nk_pin_lookup(pin_name);
    	if (pin_idx < 0) {
	    nk_printf("Unknown pin\n");
    	} else {
	    nk_pinmode_t mode = nk_pin_mode_lookup(pin_mode);
	    if (mode == NK_PINMODE_COUNT) {
	        nk_printf("Unknown mode\n");
	    } else {
	        nk_pin_setmode(&nk_pin_table[pin_idx], mode);
	    }
    	}
    } else {
    	nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pin,
    ">pin                       GPIO pin commands\n"
    "-pin                       List GPIO info\n"
    "-pin set <pin>             Set pin\n"
    "-pin clear <pin>           Clear pin\n"
    "-pin mode <mode>           Change pin mode\n"
)
