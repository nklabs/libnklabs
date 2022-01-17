// GPIO pin database

#include "nkpin.h"

// Pin table indices

enum pin_idx {
#ifdef NK_PLATFORM_STM32
#ifdef ARD_A0_Pin
    PIN_IDX_ARD_A0,
#endif
    PIN_IDX_ARD_A1,
    PIN_IDX_ARD_A2,
    PIN_IDX_ARD_A3,
    PIN_IDX_ARD_A4,
    PIN_IDX_ARD_A5,

#ifdef ARD_D0_Pin
    PIN_IDX_ARD_D0,
#endif
#ifdef ARD_D1_Pin
    PIN_IDX_ARD_D1,
#endif

    PIN_IDX_ARD_D2,
    PIN_IDX_ARD_D3,
    PIN_IDX_ARD_D4,
    PIN_IDX_ARD_D5,
    PIN_IDX_ARD_D6,
    PIN_IDX_ARD_D7,
    PIN_IDX_ARD_D8,
    PIN_IDX_ARD_D9,
    PIN_IDX_ARD_D10,
    PIN_IDX_ARD_D11,
    PIN_IDX_ARD_D12,
    PIN_IDX_ARD_D13,
#endif

#ifdef NK_PLATFORM_ATSAM
    PIN_IDX_ARD_A0,
    PIN_IDX_ARD_A1,
    PIN_IDX_ARD_A2,
    PIN_IDX_ARD_A3,
    PIN_IDX_ARD_A4,
    PIN_IDX_ARD_A5,

    PIN_IDX_ARD_D0,
    PIN_IDX_ARD_D1,
    PIN_IDX_ARD_D2,
    PIN_IDX_ARD_D3,
    PIN_IDX_ARD_D4,
    PIN_IDX_ARD_D5,
    PIN_IDX_ARD_D6,
    PIN_IDX_ARD_D7,
    PIN_IDX_ARD_D8,
    PIN_IDX_ARD_D9,
    PIN_IDX_ARD_D10,
    PIN_IDX_ARD_D11,
    PIN_IDX_ARD_D12,
    PIN_IDX_ARD_D13,
#endif

    PIN_COUNT
};
