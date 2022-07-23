#include "lib.h"

const struct regfield fpga_regtable[]=
{
    { FPGAREG_ID, 0, 32, "id", "" },
    { FPGAREG_TEST, 0, 32, "test", "" },
    { FPGAREG_INT_ENABLE, 0, 32, "int_enable", "" },
        { 0x01, 0, 1, "eth_rx_int_enable", "" },
    { FPGAREG_INT_PENDING, 0, 32, "int_pending", "" },
        { 0x01, 0, 1, "eth_rx_int_pending", "" },
    { 0, 0, 0, 0, 0 }
};

void reboot()
{
    // fpga_wr(FPGAREG_CAM_I2C, (REBOOT_BIT << 16) + REBOOT_BIT); // Reboot system
}
