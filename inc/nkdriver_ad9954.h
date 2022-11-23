#ifndef _Iad9954
#define _Iad9954

#include "nkspi.h"
#include "nkpin.h"

// First byte of SPI transfer:
//        D7: high for read, low for write
//   D6 - D5: don't care
//   D4 - D0: register address

#define AD9954_REG_CFR1 0x00 // 4 bytes
#define AD9954_REG_CFR2 0x01 // 3 bytes
#define AD9954_REG_ASF 0x02 // 2 bytes
#define AD9954_REG_ARR 0x03 // 1 byte
#define AD9954_REG_FTW0 0x04 // 4 bytes
#define AD9954_REG_POW0 0x05 // 2 bytes
#define AD9954_REG_FTW1 0x06 // 4 bytes
#define AD9954_REG_RSCW0 0x07 // 5 bytes
#define AD9954_REG_RSCW1 0x08 // 5 bytes
#define AD9954_REG_RSCW2 0x09 // 5 bytes
#define AD9954_REG_RSCW3 0x0A // 5 bytes
#define AD9954_REG_RM 0x0B // 4 bytes

typedef struct {
    nkspi_device_t *bus;
    nk_pin_t *update_pin;
    nk_pin_t *pwr_pin;
    nk_pin_t *rst_pin;
    nk_pin_t *osk_pin;
    nk_pin_t *ps0_pin;
    nk_pin_t *ps1_pin;
    nk_pin_t *iosync_pin;
} nk_ad9954_t;

int nk_ad9954_send(const nk_ad9954_t *dev, uint8_t *data, uint32_t len);
int nk_ad9954_set_freq(const nk_ad9954_t *dev, uint32_t freq);
int nk_ad9954_reset(const nk_ad9954_t *dev);
int nk_ad9954_init(const nk_ad9954_t *dev);

#endif
