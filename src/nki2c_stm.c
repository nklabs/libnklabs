// Copyright 2020 NK Labs, LLC

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

// STM32 HAL I2C

#include "nkarch.h"
#include "nki2c.h"

//#define DEBUGIT

#ifdef DEBUGIT
static const char *stm_i2c_error_string(int code)
{
	switch (code) {
		case HAL_I2C_ERROR_NONE: return "none";
		case HAL_I2C_ERROR_BERR: return "BERR";
		case HAL_I2C_ERROR_ARLO: return "ARLO";
		case HAL_I2C_ERROR_AF: return "ACKF";
		case HAL_I2C_ERROR_OVR: return "OVR";
		case HAL_I2C_ERROR_DMA: return "DMA";
		case HAL_I2C_ERROR_TIMEOUT: return "Timeout";
		case HAL_I2C_ERROR_SIZE: return "Size";
		case HAL_I2C_ERROR_DMA_PARAM: return "DMA Param";
		default: return "unknown";
	}
}
#endif

int nk_hal_i2c_write(void *port, uint8_t addr, size_t len, const uint8_t *buf)
{
	I2C_HandleTypeDef *hand = (I2C_HandleTypeDef *)port;
	int e;

	while (HAL_BUSY == (e = HAL_I2C_Master_Transmit(hand, addr * 2, buf, len, 1000)))
		nk_printf("Busy\n");

        if (HAL_OK != e) {
#ifdef DEBUGIT
                e = HAL_I2C_GetError(hand);
                nk_printf("I2C Tx failed error code = %x (%s)\n", e, stm_i2c_error_string(e));
#endif
                return -1;
        }
        return 0;
}

int nk_hal_i2c_write_nostop(void *port, uint8_t addr, size_t len, const uint8_t *buf)
{
        return nk_hal_i2c_write(port, addr, len, buf);
}

int nk_hal_i2c_read(void *port, uint8_t addr, size_t len, uint8_t *buf)
{
	I2C_HandleTypeDef *hand = (I2C_HandleTypeDef *)port;
	int e;

	while (HAL_BUSY == (e = HAL_I2C_Master_Receive(hand, addr * 2, buf, len, 1000)))
		nk_printf("Busy\n");

	if (HAL_OK != e) {
#ifdef DEBUGIT
                e = HAL_I2C_GetError(hand);
                nk_printf("I2C Rx failed error code = %x (%s)\n", e, stm_i2c_error_string(e));
#endif
		return -1;
	}
	return 0;
}
