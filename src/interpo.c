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

#include "interpo.h"

// Interpolation: table must be monotonically increasing by first column

int interpo(int nrows, struct xy *t, double in, double *out)
{
    int a, b, c;

    // Binary search for correct entry
    a = 0;
    c = b = nrows - 1;


    while (b != (a + c) / 2) {
        b = (a + c) / 2;
        if (in >= t[b].x)
            a = b;
        else if (in < t[b].x)
            c = b;
    }

    // Interpolate

    if (in >= t[b].x && in <= t[b + 1].x) {
        *out = (in - t[b].x) / (t[b + 1].x - t[b].x) * (t[b + 1].y - t[b].y) + t[b].y;
        return 0;
    }

    return -1;
}

// Inverse interpolation: table must be monotonically increasing by second column

int interpo_inv(int nrows, struct xy *t, double in, double *out)
{
    int a, b, c;

    // Binary search for correct entry
    a = 0;
    c = b = nrows - 1;


    while (b != (a + c) / 2) {
        b = (a + c) / 2;
        if (in >= t[b].y)
            a = b;
        else if (in < t[b].y)
            c = b;
    }

    // Interpolate

    if (in >= t[b].y && in <= t[b + 1].y) {
        *out = (in - t[b].y) / (t[b + 1].y - t[b].y) * (t[b + 1].x - t[b].x) + t[b].x;
        return 0;
    }

    return -1;
}

// Inverse interpolation: table must be monotonically decreasing by second column

int interpo_inv_r(int nrows, struct xy *t, double in, double *out)
{
    int a, b, c;

    // Binary search for correct entry
    a = 0;
    c = b = nrows - 1;


    while (b != (a + c) / 2) {
        b = (a + c) / 2;
        if (in <= t[b].y)
            a = b;
        else if (in > t[b].y)
            c = b;
    }

    // Interpolate

    if (in <= t[b].y && in >= t[b + 1].y) {
        *out = (in - t[b].y) / (t[b + 1].y - t[b].y) * (t[b + 1].x - t[b].x) + t[b].x;
        return 0;
    }

    return -1;
}

