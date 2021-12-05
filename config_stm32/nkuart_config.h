#include "nkarch.h" // For UART_HandleTypeDef

// Receive buffer size: set to minimum size for ZMODEM to work without errors

// #define NK_UART_RXBUF_SIZE 16384 // for 921600 Baud
#define NK_UART_RXBUF_SIZE 2048 // for 115200 Baud

// Which UART?
// #define console_uart huart2

#ifdef STM32G071xx
#define console_uart hlpuart1
extern UART_HandleTypeDef hlpuart1;
#else
#define console_uart huart2
extern UART_HandleTypeDef huart2;
#endif
