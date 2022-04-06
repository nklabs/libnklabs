// Copyright 2021 NK Labs, LLC

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

// GPIO pin database

#include "pins.h"

const nk_pin_t nk_pin_table[] = 
{
#ifdef NK_PLATFORM_STM32
#ifdef ARD_A0_Pin
    [PIN_IDX_ARD_A0] = { &nk_pin_funcs, "ard_a0", ARD_A0_GPIO_Port, ARD_A0_Pin },
#endif
    [PIN_IDX_ARD_A1] = { &nk_pin_funcs, "ard_a1", ARD_A1_GPIO_Port, ARD_A1_Pin },
    [PIN_IDX_ARD_A2] = { &nk_pin_funcs, "ard_a2", ARD_A2_GPIO_Port, ARD_A2_Pin },
    [PIN_IDX_ARD_A3] = { &nk_pin_funcs, "ard_a3", ARD_A3_GPIO_Port, ARD_A3_Pin },
    [PIN_IDX_ARD_A4] = { &nk_pin_funcs, "ard_a4", ARD_A4_GPIO_Port, ARD_A4_Pin },
    [PIN_IDX_ARD_A5] = { &nk_pin_funcs, "ard_a5", ARD_A5_GPIO_Port, ARD_A5_Pin },

#ifdef ARD_D0_Pin
    [PIN_IDX_ARD_D0] = { &nk_pin_funcs, "ard_d0", ARD_D0_GPIO_Port, ARD_D0_Pin },
#endif
#ifdef ARD_D1_Pin
    [PIN_IDX_ARD_D1] = { &nk_pin_funcs, "ard_d1", ARD_D1_GPIO_Port, ARD_D1_Pin },
#endif
    [PIN_IDX_ARD_D2] = { &nk_pin_funcs, "ard_d2", ARD_D2_GPIO_Port, ARD_D2_Pin },
    [PIN_IDX_ARD_D3] = { &nk_pin_funcs, "ard_d3", ARD_D3_GPIO_Port, ARD_D3_Pin },
#ifdef ARD_D4_Pin
    [PIN_IDX_ARD_D4] = { &nk_pin_funcs, "ard_d4", ARD_D4_GPIO_Port, ARD_D4_Pin },
#endif
#ifdef ARD_D5_Pin
    [PIN_IDX_ARD_D5] = { &nk_pin_funcs, "ard_d5", ARD_D5_GPIO_Port, ARD_D5_Pin },
#endif
    [PIN_IDX_ARD_D6] = { &nk_pin_funcs, "ard_d6", ARD_D6_GPIO_Port, ARD_D6_Pin },
#ifdef ARD_D7_Pin
    [PIN_IDX_ARD_D7] = { &nk_pin_funcs, "ard_d7", ARD_D7_GPIO_Port, ARD_D7_Pin },
#endif
#ifdef ARD_D8_Pin
    [PIN_IDX_ARD_D8] = { &nk_pin_funcs, "ard_d8", ARD_D8_GPIO_Port, ARD_D8_Pin },
#endif
    [PIN_IDX_ARD_D9] = { &nk_pin_funcs, "ard_d9", ARD_D9_GPIO_Port, ARD_D9_Pin },
    [PIN_IDX_ARD_D10] = { &nk_pin_funcs, "ard_d10", ARD_D10_GPIO_Port, ARD_D10_Pin },
    [PIN_IDX_ARD_D11] = { &nk_pin_funcs, "ard_d11", ARD_D11_GPIO_Port, ARD_D11_Pin },
    [PIN_IDX_ARD_D12] = { &nk_pin_funcs, "ard_d12", ARD_D12_GPIO_Port, ARD_D12_Pin },
    [PIN_IDX_ARD_D13] = { &nk_pin_funcs, "ard_d13", ARD_D13_GPIO_Port, ARD_D13_Pin }
#endif

#ifdef NK_PLATFORM_ATSAM
    [PIN_IDX_ARD_A0] = { &nk_pin_funcs, "ard_a0", ARD_A0 },
    [PIN_IDX_ARD_A1] = { &nk_pin_funcs, "ard_a1", ARD_A1 },
    [PIN_IDX_ARD_A2] = { &nk_pin_funcs, "ard_a2", ARD_A2 },
    [PIN_IDX_ARD_A3] = { &nk_pin_funcs, "ard_a3", ARD_A3 },
    [PIN_IDX_ARD_A4] = { &nk_pin_funcs, "ard_a4", ARD_A4 },
    [PIN_IDX_ARD_A5] = { &nk_pin_funcs, "ard_a5", ARD_A5 },

    [PIN_IDX_ARD_D0] = { &nk_pin_funcs, "ard_d0", ARD_D0 },
    [PIN_IDX_ARD_D1] = { &nk_pin_funcs, "ard_d1", ARD_D1 },
    [PIN_IDX_ARD_D2] = { &nk_pin_funcs, "ard_d2", ARD_D2 },
    [PIN_IDX_ARD_D3] = { &nk_pin_funcs, "ard_d3", ARD_D3 },
    [PIN_IDX_ARD_D4] = { &nk_pin_funcs, "ard_d4", ARD_D4 },
    [PIN_IDX_ARD_D5] = { &nk_pin_funcs, "ard_d5", ARD_D5 },
    [PIN_IDX_ARD_D6] = { &nk_pin_funcs, "ard_d6", ARD_D6 },
    [PIN_IDX_ARD_D7] = { &nk_pin_funcs, "ard_d7", ARD_D7 },
    [PIN_IDX_ARD_D8] = { &nk_pin_funcs, "ard_d8", ARD_D8 },
    [PIN_IDX_ARD_D9] = { &nk_pin_funcs, "ard_d9", ARD_D9 },
    [PIN_IDX_ARD_D10] = { &nk_pin_funcs, "ard_d10", ARD_D10 },
    [PIN_IDX_ARD_D11] = { &nk_pin_funcs, "ard_d11", ARD_D11 },
    [PIN_IDX_ARD_D12] = { &nk_pin_funcs, "ard_d12", ARD_D12 },
    [PIN_IDX_ARD_D13] = { &nk_pin_funcs, "ard_d13", ARD_D13 }
#endif
};
