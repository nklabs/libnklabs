#ifndef _Inkdriver_tm1638
#define _Inkdriver_tm1638

#include "nkpin.h"

#define TM1638_MODE_LOAD 0x40 // Set data mode to load digits
#define TM1638_MODE_READ 0x42 // Set data mode to read keyboard
// Need 2 us to switch between command output and keyboard data input

#define TM1638_ADDR 0xC0 // Set starting write address 0 - 0xF
// For common cathode LED displays:
//  There are up to 8 digits and each display has up to 10 segments.
//  Each digit has two bytes.  The bytes for a specific digit are: address[digit*2] and adress[digit*2+1]

#define TM1638_DISP_ON 0x88 // Display on, with brightness 0 - 7
#define TM1638_DISP_OFF 0x80 // Display off

typedef struct {
    const nk_pin_t *clk; // Clock pin
    const nk_pin_t *dio; // DIO pin (open drain)
    const nk_pin_t *stb; // Stobe pin

    // stb, dio and clk all typically have 100pF capacitors on them
    const int trise; // 200 us on one unit RC pull-up
    const int tfall; // 3 us on one unit C with max pin current pull-down

    // Segments seem to be connected consistently between units,
    // but digits are not.  Here is a mapping:

    uint8_t digit_map[8];
} nk_tm1638_t;

int nk_tm1638_init(const nk_tm1638_t *dev);

// Write 8 digits to TM1638
// This writes directly to the TM1638 refresh registers seguentially as is
int nk_tm1638_display_raw(const nk_tm1638_t *dev, const uint16_t *digits);

// Use built-in font to convert ASCII to 7-segments:
// ASCII 0 - 9, A - F, a - F, space and - are converted
// Otherwise use lower four bits for hexadecimal
// Digits are remapped through digit_map, so the display order is up to you
// dp is a bitmap of which decimal points to illuminate (SEG8) 
// extra1 is a bitmap of which extra segments to illuminate (SEG9)
// extra2 is a bitmap of which extra segments to illuminate (SEG10)
int nk_tm1638_display(const nk_tm1638_t *dev, uint8_t dp, uint8_t extra1, uint8_t extra2, const uint8_t *digits);

// Turn display on with brightness 0..7
int nk_tm1638_display_on(const nk_tm1638_t *dev, uint8_t brightness);

// Turn display off
int nk_tm1638_display_off(const nk_tm1638_t *dev);

// Scan keypad: returns a single 32-bit word bitmap
int nk_tm1638_keyscan(const nk_tm1638_t *dev, uint32_t *data);

#endif
