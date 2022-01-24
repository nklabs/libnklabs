#include "nkprintf.h"

int main(int argc, char *argv[])
{
    int x;
    unsigned char test_data[256];

    // nk_indent

    nk_indent(0); nk_printf("|-None\n");
    nk_indent(4); nk_printf("|-Four\n");


    // nk_byte_hex_dump

    for (x = 0; x != 256; ++x)
        test_data[x] = x;

    nk_byte_hex_dump(test_data, 0, 0, 256);

    nk_byte_hex_dump(test_data, 0, 7, 242);

    nk_byte_hex_dump(test_data, 0, 9, 238);

    nk_byte_hex_dump(test_data, 0, 7, 4);

    // nk_printf

    nk_printf("--|%f|--\n", .000120);
    nk_printf("--|%e|--\n", .000120);
    nk_printf("--|%g|--\n", .000120);
    nk_printf("--|%c|--\n", 'A');
    nk_printf("--|%d|--\n", 0x7FFFFFFe);
    nk_printf("--|%ld|--\n", (long)0x7FFFFFFe);
    nk_printf("--|%d|--\n", -0x7FFFFFFe);
    nk_printf("--|%ld|--\n", (long)-0x7FFFFFFe);
    nk_printf("--|%x|--\n", 0x7FFFFFFe);
    nk_printf("--|%x|--\n", 0xFFFFFF80);
    nk_printf("--|%X|--\n", 0xaddedfee);
    nk_printf("--|%s|--\n", "Hello, world!");
    nk_printf("--|%%|--\n");
    nk_printf("--|%-20s|--\n", "left");
    nk_printf("--|%20s|--\n", "right");
    nk_printf("--|%-20d|--\n", 12);
    nk_printf("--|%20d|--\n", 12);
    nk_printf("--|%.5d|--\n", 12);

    nk_printf("Try nkstdnull: "); nk_fprintf(nkstdnull, "What!!?"); nk_printf("\n");

    nk_printf("Try nkstderr: "); nk_fprintf(nkstderr, "Typical error message"); nk_printf("\n");
}
