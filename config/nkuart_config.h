#include "nkarch.h" // For UART_HandleTypeDef

// Receive buffer size: set to minimum size for ZMODEM to work without errors

// #define NK_UART_RXBUF_SIZE 16384 // for 921600 Baud
#define NK_UART_RXBUF_SIZE 2048 // for 115200 Baud
//#define NK_UART_RXBUF_SIZE 256 // for small systems...
