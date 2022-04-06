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

// HD44780 LCD display driver
// Also Sitronix ST7066U
// Also AiP31068 (but behind I2C or SPI interface)

#ifndef _Inkdriver_hd44780
#define _Inkdriver_hd44780

#include <stdint.h>
#include <stdbool.h>

// Pins

// There is a R/~W pin.  Low means write.
// There is an RS pin.  Low for instruction (for write) or busy flag (for read), High for data write or read

// There is an E pin: Data should be latched on falling edge
// RS, R/~W and write data should be held constance whlie E is high:
//   RS, R/~W setup before rising E: 60 ns
//   RS, R/~W hold after falling E: 20 ns
//   Data setup before falling E: 195 ns
//   Data hold after falling E: 10 ns
// E pulse width 450 ns min.  E cycle time min, is 1000 ns
//   For read:
//     Data delay after rising E is 360 ns
//     Data hold after falling E is 5 ns
// For busy check: wait 80us after last falling edge of E before checking it

// Commands

// In four bit mode, send upper bits first

#define HD44780_CLS 0x01 // Clear screen, home cursor, set I/D bit to 1, clear shift  (1.53 ms)

#define HD44780_HOME 0x02 // Home curosr (DDRAM address set to 0), clear shift (1.53 ms)

#define HD44780_MODE 0x04 // 39 us
#define HD44780_MODE_S 0x01 // Scroll to right mode when set
#define HD44780_MODE_ID 0x02 // Set for DDRAM address to increment when written (normal case).  Clear for decremenet.

#define HD44780_DISPLAY 0x08 // 39 us
#define HD44780_DISPLAY_D 0x04 // Display on
#define HD44780_DISPLAY_C 0x02 // Cursor on
#define HD44780_DISPLAY_B 0x01 // Cursor blink

#define HD44780_SHIFT 0x10 // Shift display or cursor right or left (39 us)
#define HD44780_SHIFT_RL 0x04 // Low is left, high is right
#define HD44780_SHIFT_SC 0x08 // Low is cursor, high is shift

// Always send this first, especially so that it knows the interface width
#define HD44780_FUNC 0x20 // (39 us)
#define HD44780_FUNC_DL 0x10 // Data Length: 4-bits when 0, 8-bits when 1.
#define HD44780_FUNC_N 0x08 // Set for two lines
#define HD44780_FUNC_F 0x04 // Font

#define HD44780_CGRAM 0x40 // Set character generator RAM address (lower 6 bits) (39 us)

#define HD44780_DDRAM 0x80 // Set display RAM address (lower 7 bits) (39 us)

// There are 80 bytes of display refresh RAM
// This is coniguous for a 1-line display
// For a two line display, the 2nd line start at address 0x40

// Provide mechanism to access the HD44780

typedef struct {
    int (*send_command)(void *ptr, uint8_t command); // Send command byte or command nibble (upper half)
    int (*send_data)(void *ptr, uint8_t *data, int len); // Send data bytes or data nibbles (upper half)
    void *ptr; // First argument for above functions
    bool f_bit; // Font select bit
    bool n_bit; // Number of lines select bit
    uint8_t width; // Width of display
    uint8_t height; // Height of display
    uint8_t offsets[4]; // RAM address of each line
} nk_hd44780_t;

// Setup
int nk_hd44780_setup(const nk_hd44780_t *dev);

// Set display mode
int nk_hd44780_display_mode(const nk_hd44780_t *dev, bool display_on, bool cursor_on, bool cursor_blink);

// Set cursor position
int nk_hd44780_pos(const nk_hd44780_t *dev, uint8_t position);

// Write data
int nk_hd44780_write(const nk_hd44780_t *dev, uint8_t *data, int len);

// Clear screen
int nk_hd44780_cls(const nk_hd44780_t *dev);

// Home cursor
int nk_hd44780_home(const nk_hd44780_t *dev);

#endif
