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

#include "nkfont_7seg.h"

// Standard segment names:

//     AAAA
//    F    B
//    F    B
//    F    B
//     GGGG
//    E    C
//    E    C
//    E    C
//     DDDD   DP

// ASCII to 7-segment conversion
// 0x00 - 0x0F    Converted to hexadecimal digits
// 0x10 - 0x1F    Blank
// 0x20 - 0x7F    ASCII, best effort..

const uint8_t nkfont_7seg[128] =
{
    /* 0x00 NUL */ SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    /* 0x01     */ SEG_B | SEG_C,
    /* 0x02     */ SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
    /* 0x03     */ SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,
    /* 0x04     */ SEG_F | SEG_G | SEG_B | SEG_C,
    /* 0x05     */ SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    /* 0x06     */ SEG_A | SEG_F | SEG_G | SEG_E | SEG_D | SEG_C,
    /* 0x07     */ SEG_A | SEG_B | SEG_C,
    /* 0x08     */ SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    /* 0x09     */ SEG_A | SEG_B | SEG_G | SEG_F | SEG_C,
    /* 0x0A     */ SEG_A | SEG_B | SEG_F | SEG_G | SEG_C | SEG_E,
    /* 0x0B     */ SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
    /* 0x0C     */ SEG_A | SEG_F | SEG_E | SEG_D,
    /* 0x0D     */ SEG_G | SEG_E | SEG_C | SEG_D | SEG_B,
    /* 0x0E     */ SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,
    /* 0x0F     */ SEG_A | SEG_F | SEG_G | SEG_E,

    /* 0x10     */ 0,
    /* 0x11     */ 0,
    /* 0x12     */ 0,
    /* 0x13     */ 0,
    /* 0x14     */ 0,
    /* 0x15     */ 0,
    /* 0x16     */ 0,
    /* 0x17     */ 0,
    /* 0x18     */ 0,
    /* 0x19     */ 0,
    /* 0x1A     */ 0,
    /* 0x1B     */ 0,
    /* 0x1C     */ 0,
    /* 0x1D     */ 0,
    /* 0x1E     */ 0,
    /* 0x1F     */ 0,

    /* 0x20 SPC */ 0,
    /* 0x21 !   */ SEG_B | SEG_DP,
    /* 0x22 "   */ SEG_F | SEG_B,
    /* 0x23 #   */ 0,
    /* 0x24 $   */ 0,
    /* 0x25 %   */ 0,
    /* 0x26 &   */ 0,
    /* 0x27 '   */ SEG_F,
    /* 0x28 (   */ 0,
    /* 0x29 )   */ 0,
    /* 0x2A *   */ 0,
    /* 0x2B +   */ 0,
    /* 0x2C ,   */ SEG_E,
    /* 0x2D -   */ SEG_G,
    /* 0x2E .   */ SEG_DP,
    /* 0x2F /   */ SEG_B | SEG_G | SEG_E,

    /* 0x30 0   */ SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,
    /* 0x31 1   */ SEG_B | SEG_C,
    /* 0x32 2   */ SEG_A | SEG_B | SEG_G | SEG_E | SEG_D,
    /* 0x33 3   */ SEG_A | SEG_B | SEG_G | SEG_C | SEG_D,
    /* 0x34 4   */ SEG_F | SEG_G | SEG_B | SEG_C,
    /* 0x35 5   */ SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    /* 0x36 6   */ SEG_A | SEG_F | SEG_G | SEG_E | SEG_D | SEG_C,
    /* 0x37 7   */ SEG_A | SEG_B | SEG_C,
    /* 0x38 8   */ SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G,
    /* 0x39 9   */ SEG_A | SEG_B | SEG_G | SEG_F | SEG_C,
    /* 0x3A :   */ 0,
    /* 0x3B ;   */ 0,
    /* 0x3C <   */ 0,
    /* 0x3D =   */ SEG_G | SEG_D,
    /* 0x3E >   */ 0,
    /* 0x3F ?   */ SEG_F | SEG_A | SEG_B | SEG_C | SEG_DP,

    /* 0x40 @   */ 0,
    /* 0x41 A   */ SEG_A | SEG_B | SEG_F | SEG_G | SEG_C | SEG_E,
    /* 0x42 B   */ SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
    /* 0x43 C   */ SEG_A | SEG_F | SEG_E | SEG_D,
    /* 0x44 D   */ SEG_G | SEG_E | SEG_C | SEG_D | SEG_B,
    /* 0x45 E   */ SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,
    /* 0x46 F   */ SEG_A | SEG_F | SEG_G | SEG_E,
    /* 0x47 G   */ SEG_A | SEG_F | SEG_B | SEG_G | SEG_C | SEG_D,
    /* 0x48 H   */ SEG_F | SEG_B | SEG_G | SEG_E | SEG_C,
    /* 0x49 I   */ SEG_B | SEG_C,
    /* 0x4A J   */ SEG_B | SEG_C | SEG_D,
    /* 0x4B K   */ SEG_F | SEG_G | SEG_E,
    /* 0x4C L   */ SEG_F | SEG_E | SEG_D,
    /* 0x4D M   */ 0,
    /* 0x4E N   */ SEG_E | SEG_G | SEG_C,
    /* 0x4F O   */ SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,

    /* 0x50 P   */ SEG_A | SEG_F | SEG_B | SEG_G | SEG_E,
    /* 0x51 Q   */ SEG_G | SEG_E | SEG_C | SEG_D | SEG_DP,
    /* 0x52 R   */ SEG_E | SEG_G,
    /* 0x53 S   */ SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    /* 0x54 T   */ SEG_B | SEG_G | SEG_C,
    /* 0x55 U   */ SEG_E | SEG_D | SEG_C | SEG_F | SEG_B,
    /* 0x56 V   */ 0,
    /* 0x57 W   */ 0,
    /* 0x58 X   */ 0,
    /* 0x59 Y   */ SEG_F | SEG_G | SEG_B | SEG_C | SEG_D,
    /* 0x5A Z   */ 0,
    /* 0x5B [   */ 0,
    /* 0x5C \   */ SEG_F | SEG_G | SEG_C,
    /* 0x5D ]   */ 0,
    /* 0x5E ^   */ 0,
    /* 0x5F _   */ SEG_D,

    /* 0x60 `   */ SEG_B,
    /* 0x61 a   */ SEG_A | SEG_B | SEG_F | SEG_G | SEG_C | SEG_E,
    /* 0x62 b   */ SEG_F | SEG_E | SEG_D | SEG_C | SEG_G,
    /* 0x63 c   */ SEG_G | SEG_E | SEG_D,
    /* 0x64 d   */ SEG_G | SEG_E | SEG_C | SEG_D | SEG_B,
    /* 0x65 e   */ SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,
    /* 0x66 f   */ SEG_A | SEG_F | SEG_G | SEG_E,
    /* 0x67 g   */ SEG_A | SEG_F | SEG_B | SEG_G | SEG_C | SEG_D,
    /* 0x68 h   */ SEG_F | SEG_G | SEG_E | SEG_C,
    /* 0x69 i   */ SEG_C,
    /* 0x6A j   */ SEG_B | SEG_C | SEG_D,
    /* 0x6B k   */ SEG_F | SEG_G | SEG_E,
    /* 0x6C l   */ SEG_F | SEG_E,
    /* 0x6D m   */ 0,
    /* 0x6E n   */ SEG_E | SEG_G | SEG_C,
    /* 0x6F o   */ SEG_G | SEG_E | SEG_C | SEG_D,

    /* 0x70 p   */ SEG_A | SEG_F | SEG_B | SEG_G | SEG_E,
    /* 0x71 q   */ SEG_G | SEG_E | SEG_C | SEG_D | SEG_DP,
    /* 0x72 r   */ SEG_E | SEG_G,
    /* 0x73 s   */ SEG_A | SEG_F | SEG_G | SEG_C | SEG_D,
    /* 0x74 t   */ SEG_B | SEG_G | SEG_C,
    /* 0x75 u   */ SEG_E | SEG_D | SEG_C,
    /* 0x76 v   */ 0,
    /* 0x77 w   */ 0,
    /* 0x78 x   */ 0,
    /* 0x79 y   */ SEG_F | SEG_G | SEG_B | SEG_C | SEG_D,
    /* 0x7A z   */ 0,
    /* 0x7B {   */ 0,
    /* 0x7C |   */ 0,
    /* 0x7D }   */ 0,
    /* 0x7E ~   */ 0,
    /* 0x7F DEL */ 0
};
