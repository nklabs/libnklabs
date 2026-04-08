#include "nkdriver_tcal6408.h"

int tcal6408_get_input(const nk_i2c_device_t *dev, uint8_t pin, uint8_t *result){
    uint8_t byte;
    if (pin > 7) return -2;
    int status = nk_i2c_get_byte(dev, TCAL6408_INPUT_PORT, &byte);
    if (status) return status;
    *result = (byte >> pin) & 1;
    return 0;
}

int tcal6408_set_output(const nk_i2c_device_t *dev, uint8_t pin, uint8_t val){
    uint8_t byte;
    int status = nk_i2c_get_byte(dev, TCAL6408_OUTPUT_PORT, &byte);
    if (pin > 7) return -2;
    if (status) return status;
    if (val) byte |= (1 << pin);
    else byte &= ~(1 << pin);
    return nk_i2c_put_byte(dev, TCAL6408_OUTPUT_PORT, byte);
}

int tcal6408_read(const nk_i2c_device_t *dev, uint8_t addr, uint8_t *result){
    int status = nk_i2c_get_byte(dev, addr, result);
    if (status) return status;
    return 0;
}

int tcal6408_write(const nk_i2c_device_t *dev, uint8_t addr, uint8_t val) {
    return nk_i2c_put_byte(dev, addr, val);
}

int bin2dec(const char bits[], uint8_t *result) { 
    int index = 0;  // leftmost bit most significant
    int rtn = 0;

    while(bits[index] != '\0') {
        if (bits[index] != '0' && bits[index] != '1') {
            return -1;  // invalid input
        }
        if (index >= 8) {
            return -1;  // input too long
        }
        rtn = (rtn << 1) | (bits[index] == '1');
        index++;
    }

    if (index == 0) return -1; // empty input

    *result = rtn;
    return 0;
}

int hex2dec(const char hex[], uint8_t *result) {
    int index = 0; 
    int rtn = 0; 

    while (hex[index] != '\0' && index < 2)
    {
        /* code */
        if ((hex[index] >= '0' && hex[index] <= '9') || (hex[index] >= 'a' && hex[index] <= 'f') || (hex[index] >= 'A' && hex[index] <= 'F')) {
            rtn = (rtn << 4) | (hex[index] <= '9' ? hex[index] - '0' : (hex[index] <= 'F' ? hex[index] - 'A' + 10 : hex[index] - 'a' + 10));
        }
        else {
            return -1; // invalid input
        }
        index++;
    }
    *result = rtn;
    return 0;
}
