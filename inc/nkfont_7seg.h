#ifndef _Inkfont_7seg
#define _Inkfont_7seg

#include <stdint.h>

#define SEG_A (0x01)
#define SEG_B (0x02)
#define SEG_C (0x04)
#define SEG_D (0x08)
#define SEG_E (0x10)
#define SEG_F (0x20)
#define SEG_G (0x40)
#define SEG_DP (0x80)

extern const uint8_t nkfont_7seg[];

#endif
