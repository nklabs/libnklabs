#ifndef _Inkdriver_tm1637
#define _Inkdriver_tm1637

#include "nkpin.h"

#define TM1637_MODE 0x40
#define TM1637_ADDR 0xC0
#define TM1637_DISP 0x88

typedef struct {
    const nk_pin_t *clk;
    const nk_pin_t *dio;
} nk_tm1637_t;

int nk_tm1637_init(const nk_tm1637_t *dev);

// Write up to 6 digits to TM1637
// Each byte in digits should be 0 - F, or anything else for blank
// Bright should be 0 - 7
int nk_tm1637_display(const nk_tm1637_t *dev, uint8_t bright, const uint8_t *digits, int len);

#endif
