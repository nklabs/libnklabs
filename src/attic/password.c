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

/* Password checking functions */

#include "lib.h"

char default_password[] = "6DD31D807FD05041051F080F766F94C591C4B8397FA66CF5FC8AB9854050978A";
char default_salt[] = "00000040C6C84F3A000000000000000000000000000000000000000000000000";

/* Convert ASCII HEX digital to binary, or -1 if invalid */

int hex_digit(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else
        return -1;
}

/* Convert 2 ASCII HEX digits to a byte or -1 if invalid */

int hex_byte(const char *s)
{
    int a = hex_digit(s[0]);
    int b = -1;
    if (s[0])
        b = hex_digit(s[1]);

    if (a != -1 && b != -1)
        return (a << 4) + b;

    return -1;
}

/* Convert ASCII hex string to binary block */

size_t hex_to_bin(uint8_t *out, size_t out_size, const char *hex)
{
    size_t org_size = out_size;
    int b;
    while (out_size && ((b = hex_byte(hex)) != -1)) {
        *out++ = b;
        hex += 2;
        --out_size;
    }
    return org_size - out_size;
}

/* Convert binary nibble to ASCII hexadecimal character */

char digit_hex(uint8_t n)
{
    if (n < 10)
        return n + '0';
    else
        return n - 10 + 'A';
}

/* Convert binary block to ASCII hex string */

size_t bin_to_hex(char *out, size_t out_size, uint8_t *data, size_t data_size)
{
    size_t org_size = out_size;
    while (out_size > 2 && data_size != 0) {
        *out++ = digit_hex(*data >> 4);
        *out++ = digit_hex(*data & 0x0F);
        out_size -= 2;
        ++data;
        --data_size;
    }
    if (out_size) {
        *out++ = 0;
        --out_size;
    }
    return org_size - out_size;
}

// Return true if password matches

int password_check(const char *entered_pwd)
{
    char *hex_saved_pwd = cal.password;
    char *hex_salt = cal.password_salt;
    uint8_t salt[SHA256_DIGEST_SIZE];
    uint8_t saved_pwd[SHA256_DIGEST_SIZE];

    memset(salt, 0, sizeof(salt));
    memset(saved_pwd, 0, sizeof(saved_pwd));

    struct sha256_ctx ctx;
    uint8_t *hash;
    hex_to_bin(salt, sizeof(salt), hex_salt);
    hex_to_bin(saved_pwd, sizeof(saved_pwd), hex_saved_pwd);

    SHA256_init(&ctx);
    SHA256_update(&ctx, salt, sizeof(salt));
    SHA256_update(&ctx, (uint8_t *)entered_pwd, strlen(entered_pwd));
    hash = SHA256_final(&ctx);

    return !memcmp(hash, saved_pwd, sizeof(saved_pwd));
}

// ARM cycle counter
volatile unsigned int *DWT_CYCCNT = (volatile unsigned int *)0xE0001004;
volatile unsigned int *DWT_CONTROL = (volatile unsigned int *)0xE0001000;
volatile unsigned int *SCB_DEMCR = (volatile unsigned int *)0xE000EDFC;

void reset_timer(){
    *SCB_DEMCR   = *SCB_DEMCR | 0x01000000;
    *DWT_CYCCNT  = 0; // reset the counter
    *DWT_CONTROL = 0; 
}

void start_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 1 ; // enable the counter
}

void stop_timer(){
    *DWT_CONTROL = *DWT_CONTROL | 0 ; // disable the counter    
}

unsigned int getCycles(){
    return *DWT_CYCCNT;
}


// Set a new password

void password_set(const char *entered_pwd)
{
    char *hex_salt = cal.password_salt;
    char *hex_saved_pwd = cal.password;

    uint8_t salt[SHA256_DIGEST_SIZE];
    struct sha256_ctx ctx;
    uint8_t *hash;

    // Put random data into salt
    memset(salt, 0, sizeof(salt));
    *(uint32_t *)salt = getCycles();
    *((uint32_t *)salt + 1) = Seconds_get();

    bin_to_hex(hex_salt, sizeof(hex_salt), salt, sizeof(salt));

    SHA256_init(&ctx);
    SHA256_update(&ctx, salt, sizeof(salt));
    SHA256_update(&ctx, (uint8_t *)entered_pwd, strlen(entered_pwd));
    hash = SHA256_final(&ctx);

    bin_to_hex(hex_saved_pwd, sizeof(hex_saved_pwd), hash, SHA256_DIGEST_SIZE);
}
