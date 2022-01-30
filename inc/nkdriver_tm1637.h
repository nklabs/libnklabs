#ifndef _Inkdriver_tm1637
#define _Inkdriver_tm1637

#include "nkpin.h"

#define TM1637_MODE_LOAD 0x40 // Set data mode to load digits
#define TM1637_MODE_READ 0x42 // Set data mode to read keyboard
#define TM1637_ADDR 0xC0 // Set starting write address to 0
#define TM1637_DISP_ON 0x88 // Display on, set brightness 0 - 7
#define TM1637_DISP_OFF 0x80 // Display off

typedef struct {
    const nk_pin_t *clk;
    const nk_pin_t *dio;

    // TM1637 boards I've seen have large capacitors on CLK and DIA,
    // so the speed is limited by the rise/fall times on these lines.

    // TM1637 itself is pretty fast, but the boards must have noise
    // problems.

    const int trise; // 200 us on one unit RC pull-up
    const int tfall; // 3 us on one unit C with max pin current pull-down

    // Segments seem to be connected consistently between units,
    // but digits are not.  Here is a mapping:

    uint8_t digit_map[6];
} nk_tm1637_t;

int nk_tm1637_init(const nk_tm1637_t *dev);

// Write 6 digits to TM1637
// This writes directly to the TM1637 refresh registers seguentially as is
// Bright should be 0 - 7
int nk_tm1637_display_raw(const nk_tm1637_t *dev, uint8_t bright, const uint8_t *digits);

// Use built-in font to convert ASCII to 7-segments:
// ASCII 0 - 9, A - F, a - F, space and - are converted
// Otherwise use lower four bits for hexadecimal
// Digits are remapped through digit_map, so the display order is up to you
int nk_tm1637_display(const nk_tm1637_t *dev, uint8_t bright, const uint8_t *digits);

#endif
