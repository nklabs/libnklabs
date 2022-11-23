#include "nkdriver_ad9954.h"

// Pulse iosync to reset serial communications, then send data

int nk_ad9954_send(const nk_ad9954_t *dev, uint8_t *data, uint32_t len)
{
    int rtn;
    rtn = nk_pin_write(dev->iosync_pin, 1);
    rtn |= nk_pin_write(dev->iosync_pin, 0);
    rtn |= nk_spi_transfer(dev->bus, data, len);
    return rtn;
}

int nk_ad9954_set_freq(const nk_ad9954_t *dev, uint32_t freq)
{
    int rtn;
    uint8_t data[5];
    freq = freq * 10.73741824;
    data[0] = 0x04;
    data[1] = (freq >> 24);
    data[2] = (freq >> 16);
    data[3] = (freq >> 8);
    data[4] = (freq);
    rtn = nk_ad9954_send(dev, data, 5);
    rtn |= nk_pin_write(dev->update_pin, 1);
    rtn |= nk_pin_write(dev->update_pin, 0);
    return rtn;
}

int nk_ad9954_reset(const nk_ad9954_t *dev)
{
    int rtn;
    uint8_t data[5];
    rtn = nk_pin_write(dev->rst_pin, 1);
    rtn |= nk_pin_write(dev->rst_pin, 0);
    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0x22;
    data[4] = 0;
    rtn |= nk_ad9954_send(dev, data, 5);
    data[0] = 0x01;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0xA4;
    rtn |= nk_ad9954_send(dev, data, 4);
    return rtn;
}

int nk_ad9954_init(const nk_ad9954_t *dev)
{
    int rtn;
    rtn = nk_pin_write(dev->bus->cs_pin, 0);
    rtn |= nk_pin_write(dev->update_pin, 0);
    rtn |= nk_pin_write(dev->pwr_pin, 0);
    rtn |= nk_pin_write(dev->rst_pin, 0);
    rtn |= nk_pin_write(dev->osk_pin, 0);
    rtn |= nk_pin_write(dev->ps0_pin, 0);
    rtn |= nk_pin_write(dev->ps1_pin, 0);
    rtn |= nk_pin_write(dev->iosync_pin, 0);
    return rtn;
}
