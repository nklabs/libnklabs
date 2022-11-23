// Some kind of display

#ifndef _Inkdisplay
#define _Inkdisplay

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    // Low level access
    int (*init)(void *ptr); // Initialize
    int (*cls)(void *ptr); // Clear display, home cursor
    int (*write_raw)(void *ptr, int x, int y, uint8_t *data, int len); // Load display RAM with raw data beginning at row y, column x
    int (*write)(void *ptr, int x, int y, uint8_t *data, int len); // Load display RAM with ASCII beginning at row y, column x
    int (*cursor)(void *ptr, int x, int y); // Set cursor row, column position
    int (*mode)(void *ptr, bool display_on, bool cursor_on, bool cursor_blink, int brightness); // Set display mode
    int (*scan)(void *ptr, uint8_t *buf); // Keyboard scan
    void *ptr;

    // Display size
    int width;
    int height;

    // Write buffer
    char *buf;
} nkdisplay_t;

#endif
