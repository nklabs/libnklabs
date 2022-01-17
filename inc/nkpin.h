// Abstracted GPIO pin

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
    bool (*pin_read)(const nk_pin_t *pin);
    void (*pin_write)(const nk_pin_t *pin, bool val);
    void (*pin_setmode)(const nk_pin_t *pin, nk_pinmode_t mode);
    nk_pinmode_t (*pin_getmode)(const nk_pin_t *pin);
};

extern const nk_pin_func_t nk_pin_funcs;

// User provides this table

extern const nk_pin_t nk_pin_table[];

// User functions

// Read pin
#define nk_pin_read(pin) (pin)->func->pin_read(pin)

// Write pin
#define nk_pin_write(pin, val) (pin)->func->pin_write((pin), (val))

// Set up pin for a specific mode
#define nk_pin_setmode(pin, mode) (pin)->func->pin_setmode((pin), (mode))

// Get current mode
#define nk_pin_getmmode(pin) (pin)->func->pin_getmode(pin)

#endif
