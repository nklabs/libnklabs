#include <string.h>
#include <stddef.h>
#include "nkprintf.h"
#include "nkscan.h"

int main(int argc, char *argv[])
{
    int n;
    double d;
    nkinfile_t f;
    char buf[80];

    nkinfile_open_string(&f, " \t\r\nX");
    printf("nk_fscan_ws_1: ");
    if (nk_fscan_ws(&f)) {
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    nkinfile_open_string(&f, "X");
    printf("nk_fscan_ws_2: ");
    if (nk_fscan_ws(&f)) {
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }


    // Word ending on a space
    nkinfile_open_string(&f, "this-is-a-word but-not-this");
    printf("nk_fscan_word_1: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this-is-a-word");
    printf("nk_fscan_word_2: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but-not-this");
    printf("nk_fscan_word_3: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_word_4: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but width set to 4
    // This limits the number of chars taken from the input unless input is a string-word

    // Word ending on a space
    nkinfile_open_string(&f, "this-is-a-word but-not-this");
    printf("nk_fscan_word_5: ");
    if (nk_fscan_word(&f, buf, 4, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this-is-a-word");
    printf("nk_fscan_word_6: ");
    if (nk_fscan_word(&f, buf, 4, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but-not-this");
    printf("nk_fscan_word_7: ");
    if (nk_fscan_word(&f, buf, 4, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_word_8: ");
    if (nk_fscan_word(&f, buf, 4, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but output width limited to 4

    // Word ending on a space
    nkinfile_open_string(&f, "this-is-a-word but-not-this");
    printf("nk_fscan_word_9: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this-is-a-word");
    printf("nk_fscan_word_10: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but-not-this");
    printf("nk_fscan_word_11: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_word_12: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but output buffer limited to 4 and input string is 3

    // Word ending on a space
    nkinfile_open_string(&f, "thi but-not-this");
    printf("nk_fscan_word_13: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "thi");
    printf("nk_fscan_word_14: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"thi\" but-not-this");
    printf("nk_fscan_word_15: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"thi\"");
    printf("nk_fscan_word_16: ");
    buf[3] = 'A';
    if (nk_fscan_word(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Failed case, starts with space
    nkinfile_open_string(&f, " but-not-this");
    printf("nk_fscan_word_17: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    // Failed case, bad string
    nkinfile_open_string(&f, "\"missing close");
    printf("nk_fscan_word_18: ");
    if (nk_fscan_word(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }



    // Word ending on a space
    nkinfile_open_string(&f, "This_is_a_word123 but_not_this");
    printf("nk_fscan_ident_1: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this_is_a_word");
    printf("nk_fscan_ident_2: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but-not-this");
    printf("nk_fscan_ident_3: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_ident_4: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but width set to 4
    // This limits the number of chars taken from the input unless input is a string-word

    // Word ending on a space
    nkinfile_open_string(&f, "this_is_a_word but_not_this");
    printf("nk_fscan_ident_5: ");
    if (nk_fscan_ident(&f, buf, 4, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this_is_a_word");
    printf("nk_fscan_ident_6: ");
    if (nk_fscan_ident(&f, buf, 4, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but_not_this");
    printf("nk_fscan_ident_7: ");
    if (nk_fscan_ident(&f, buf, 4, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_ident_8: ");
    if (nk_fscan_ident(&f, buf, 4, sizeof(buf))) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but output width limited to 4

    // Word ending on a space
    nkinfile_open_string(&f, "this_is_a_word but_not_this");
    printf("nk_fscan_ident_9: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "this_is_a_word");
    printf("nk_fscan_ident_10: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"this is a word\" but-not-this");
    printf("nk_fscan_ident_11: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"this is a word\"");
    printf("nk_fscan_ident_12: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Same as above, but output buffer limited to 4 and input string is 3

    // Word ending on a space
    nkinfile_open_string(&f, "thi but_not_this");
    printf("nk_fscan_ident_13: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Make sure word can end at EOF
    nkinfile_open_string(&f, "thi");
    printf("nk_fscan_ident_14: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Word can be a string
    nkinfile_open_string(&f, "\"thi\" but_not_this");
    printf("nk_fscan_ident_15: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Check that string word can be at EOF
    nkinfile_open_string(&f, "\"thi\"");
    printf("nk_fscan_ident_16: ");
    buf[3] = 'A';
    if (nk_fscan_ident(&f, buf, -1, 4)) {
        printf(" got '%s' rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return\n");
    }

    // Failed case, starts with space
    nkinfile_open_string(&f, " but_not_this");
    printf("nk_fscan_ident_17: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    // Failed case, bad string
    nkinfile_open_string(&f, "\"missing close");
    printf("nk_fscan_ident_18: ");
    if (nk_fscan_ident(&f, buf, -1, sizeof(buf))) {
        printf(" got %s rest is ", buf);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    uint64_t val = 0;

    // Hex
    nkinfile_open_string(&f, "AddedFee Rest");
    printf("nk_fscan_hex_1: ");
    if (nk_fscan_hex(&f, &val, -1)) {
        printf("Got %llx rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    val = 0;
    nkinfile_open_string(&f, "FfAa90");
    printf("nk_fscan_hex_2: ");
    if (nk_fscan_hex(&f, &val, -1)) {
        printf("Got %llx rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    val = 0;
    nkinfile_open_string(&f, " FfAa90");
    printf("nk_fscan_hex_3: ");
    if (nk_fscan_hex(&f, &val, -1)) {
        printf("Got %llx rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    // Decimal
    val = 0;
    nkinfile_open_string(&f, "9810 Rest");
    printf("nk_fscan_dec_1: ");
    if (nk_fscan_dec(&f, &val, -1)) {
        printf("Got %lld rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    val = 0;
    nkinfile_open_string(&f, "9810");
    printf("nk_fscan_dec_2: ");
    if (nk_fscan_dec(&f, &val, -1)) {
        printf("Got %lld rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    val = 0;
    nkinfile_open_string(&f, " 9810");
    printf("nk_fscan_dec_3: ");
    if (nk_fscan_dec(&f, &val, -1)) {
        printf("Got %lld rest is ", (unsigned long long)val);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    // Double
    double dval = 0.0;

    nkinfile_open_string(&f, "98.10 Rest");
    printf("nk_fscan_double_1: ");
    if (nk_fscan_double(&f, &dval)) {
        printf("Got %g rest is ", dval);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    dval = 0.0;

    nkinfile_open_string(&f, "98.10");
    printf("nk_fscan_double_2: ");
    if (nk_fscan_double(&f, &dval)) {
        printf("Got %g rest is ", dval);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }

    dval = 0.0;

    nkinfile_open_string(&f, " 98.10");
    printf("nk_fscan_double_3: ");
    if (nk_fscan_double(&f, &dval)) {
        printf("Got %g rest is ", dval);
        nk_infile_print(&f); printf("\n");
    } else {
        printf("Bad return ");
        nk_infile_print(&f); printf("\n");
    }


    // Escape sequences
    nkinfile_open_string(&f, "\"\\a\\b\\e\\f\\n\\r\\t\\\\\\xff\\x01\\xfe\\xFF\\xFE\\xfhello\"");
    printf("escape: ");
    n = nk_fscan(&f, "%w", buf, sizeof(buf));
    printf("status = %d\n", n);

    nk_byte_hex_dump(buf, 0, 0, strlen(buf));

    // Try all fscan formats
    {
    unsigned a;
    int b;
    unsigned char c;
    char d;
    unsigned short e;
    short fi;
    unsigned long g;
    long h;
    unsigned long long h1;
    long long h2;
    size_t i;
    ptrdiff_t j;
    unsigned char k;
    unsigned short l;
    unsigned long m;
    unsigned long long m1;
    void *n;
    char wbuf[20];
    char ibuf[20];
    double o;

    nkinfile_open_string(&f, "4294967280 2147483632 240");
    int rtn = nk_fscan(&f, "%u %d %hhu", &a, &b, &c);
    printf("fscan1 = %d\n", rtn);
    printf("%u %d %u\n", a, b, c);

    nkinfile_open_string(&f, "120 65520 32752");
    rtn = nk_fscan(&f, "%hhd %hu %hd", &d, &e, &fi);
    printf("fscan2 = %d\n", rtn);
    printf("%d %u %d\n", d, e, fi);

    nkinfile_open_string(&f, "4294967280 2147483632 18446744073709551600");
    rtn = nk_fscan(&f, "%lu %ld %llu", &g, &h, &h1);
    printf("fscan3 = %d\n", rtn);
    printf("%lu %ld %llu\n", g, h, h1);

    nkinfile_open_string(&f, "9223372036854775792  FFFFFFF0 7FFFFFF0");
    rtn = nk_fscan(&f, "%lld %zx %tx", &h2, &i, &j);
    printf("fscan4 = %d\n", rtn);
    printf("%lld %zx %tx\n", h2, i, j);

    nkinfile_open_string(&f, "F0 FFF0 FFFFFFF0");
    rtn = nk_fscan(&f, "%hhx %hx %lx", &k, &l, &m);
    printf("fscan5 = %d\n", rtn);
    printf("%x %x %lx\n", k, l, m);

    nkinfile_open_string(&f, "FFFFFFFFFFFFFFF0 addedfee hello-world");
    rtn = nk_fscan(&f, "%llx %p %w", &m1, &n, wbuf, sizeof(wbuf));
    printf("fscan6 = %d\n", rtn);
    printf("%llx %p %s\n", m1, n, wbuf); // %p is auto-prefixed with 0x

    nkinfile_open_string(&f, "hello_world 123.456");
    rtn = nk_fscan(&f, "%i %f", ibuf, sizeof(ibuf), &o);
    printf("fscan7 = %d\n", rtn);
    printf("%s %g\n", ibuf, o); // %p is auto-prefixed with 0x
    }

    // Try %e
    nkinfile_open_string(&f, "first second");
    int rtn = nk_fscan(&f, "%i %e", buf, sizeof(buf));
    printf("fscan8 = %d, %s\n", rtn, buf);
    rtn = nk_fscan(&f, "%i", buf, sizeof(buf));
    printf("fscan8 = %d, %s ", rtn, buf);
    nk_infile_print(&f); printf("\n");

    // Try no %e, f should be left at start
    buf[0] = 0;
    nkinfile_open_string(&f, "first second");
    rtn = nk_fscan(&f, "%i ", buf, sizeof(buf));
    printf("fscan9 = %d, %s -- ", rtn, buf);
    nk_infile_print(&f); printf("\n");
    
    nkinfile_open_string(&f, "12 rest");

    if (nk_fscan(&f, "%d %e", &n))
    {
        nk_printf("got %d ", n);
        while (!nk_feof(&f)) {
            nk_printf("%c", nk_fgetc(&f));
        }
        nk_printf("\n");
    }

    nkinfile_open_string(&f, "12.5");

    if (nk_fscan(&f, "%g", &d))
    {
        nk_printf("got %g\n", d);
    }
}
