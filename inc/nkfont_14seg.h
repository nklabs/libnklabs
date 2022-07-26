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

#ifndef _Inkfont_14seg
#define _Inkfont_14seg

#include <stdint.h>

#define SEG14_A (0x0001)
#define SEG14_B (0x0002)
#define SEG14_C (0x0004)
#define SEG14_D (0x0008)
#define SEG14_E (0x0010)
#define SEG14_F (0x0020)
#define SEG14_G1 (0x0040)
#define SEG14_G2 (0x0080)
#define SEG14_H (0x0100)
#define SEG14_J (0x0200)
#define SEG14_K (0x0400)
#define SEG14_L (0x0800)
#define SEG14_M (0x1000)
#define SEG14_N (0x2000)
#define SEG14_DP (0x4000)

extern const uint16_t nkfont_14seg[];

#endif
