extern const struct regfield fpga_regtable[];

#define FPGA_BASE 0x03000000

#define FPGAREG_ID 0x0000
#define FPGAREG_TEST 0x0004

#define FPGAREG_INT_ENABLE 0x0008
#define FPGAREG_INT_PENDING 0x000c

#define ETH_RX_IRQ 0x1 // Ethernet Rx data available interrupt

void reboot();
