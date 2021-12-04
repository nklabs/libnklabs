#ifndef _Inki2c
#define _Inki2c

#include <stdint.h>
#include "nkcli.h"

// For bitbang I2C

void nk_i2c_scl_low(void *port);
void nk_i2c_scl_high(void *port);
int nk_i2c_scl_peek(void *port);

void nk_i2c_sda_low(void *port);
void nk_i2c_sda_high(void *port);
int nk_i2c_sda_peek(void *port);

// Raw I2C access

int nk_i2c_write(void *port, uint8_t addr, size_t len, const uint8_t *buf);
int nk_i2c_write_nostop(void *port, uint8_t addr, size_t len, const uint8_t *buf);
int nk_i2c_read(void *port, uint8_t addr, size_t len, uint8_t *buf);

// Register interface on top of direct access

int nk_i2c_put_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t data);
int nk_i2c_get_byte(void *port, uint8_t addr, uint8_t ofst, uint8_t *data);

int nk_i2c_put2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t data);
int nk_i2c_get2_byte(void *port, uint8_t addr, uint16_t ofst, uint8_t *data);

int nk_i2c_put_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data);
int nk_i2c_get_leshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data);
int nk_i2c_get2_leshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data);

int nk_i2c_put_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t data);
int nk_i2c_get_beshort(void *port, uint8_t addr, uint8_t ofst, uint16_t *data);

int nk_i2c_put2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t data);
int nk_i2c_get2_beshort(void *port, uint8_t addr, uint16_t ofst, uint16_t *data);

int nk_i2c_put2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t data);
int nk_i2c_get2_le24(void *port, uint8_t addr, uint16_t ofst, uint32_t *data);

int nk_i2c_put2_melong(void *port, uint8_t addr, uint16_t ofst, uint32_t data);

int nk_i2c_command(void *port, nkinfile_t *args);

#endif
