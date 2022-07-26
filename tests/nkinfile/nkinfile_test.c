#include <stdio.h>
#include <string.h>
#include "nkinfile.h"

size_t my_block_read_1(void *block_read_ptr, size_t offset, unsigned char *buffer, size_t size)
{
    char *t = (char *)block_read_ptr;
    int l = strlen(t);

    if (offset < l)
    {
        buffer[0] = t[offset];
        return 1;
    }
    else
    {
        return 0;
    }
}

size_t my_block_read_2(void *block_read_ptr, size_t offset, unsigned char *buffer, size_t size)
{
    char *t = (char *)block_read_ptr;
    int l = strlen(t);

    if (offset < l)
    {
        buffer[0] = t[offset];
        buffer[1] = t[offset + 1];
        if (offset + 1 < l)
            return 2;
        else
            return 1;
    }
    else
    {
        return 0;
    }
}

int main(int argc, char *argv[])
{
    unsigned char buf[2];
    nkinfile_t f;
    int c;
    int x;

    printf("Simple string, test nk_fgetc: ");
    nkinfile_open_string(&f, "Hello world!\n");

    while (!nk_feof(&f))
        printf("%c", nk_fgetc(&f));

    printf("Simple string, test nk_fnext: ");

    nk_fseek(&f, 0);

    for (c = nk_fpeek(&f); c != -1; c = nk_fnext(&f))
        printf("%c", c);

    printf("Simple string, test nk_fpeek_abs: ");

    nk_fseek(&f, 0);

    for (x = 0; x != 13; ++x)
        printf("%c", nk_fpeek_abs(&f, x));

    printf("..check eof: %d\n", nk_fpeek_abs(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Simple string, test nk_fpeek_rel: ");

    nk_fseek(&f, 6);

    for (x = -6; x != 7; ++x)
        printf("%c", nk_fpeek_rel(&f, x));

    printf("..check eof: %d\n", nk_fpeek_rel(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));


    printf("Blocks, test nk_fgetc: ");
    nkinfile_open(&f, my_block_read_1, "Hello world!\n", 1, buf);

    while (!nk_feof(&f))
        printf("%c", nk_fgetc(&f));

    printf("Blocks, test nk_fnext: ");

    nk_fseek(&f, 0);

    for (c = nk_fpeek(&f); c != -1; c = nk_fnext(&f))
        printf("%c", c);

    printf("Blocks, test nk_fpeek_abs: ");

    nk_fseek(&f, 0);

    for (x = 0; x != 13; ++x)
        printf("%c", nk_fpeek_abs(&f, x));

    printf("..check eof: %d\n", nk_fpeek_abs(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Blocks, test nk_fpeek_rel: ");

    nk_fseek(&f, 6);

    for (x = -6; x != 7; ++x)
        printf("%c", nk_fpeek_rel(&f, x));

    printf("..check eof: %d\n", nk_fpeek_rel(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Blocks2, test nk_fgetc: ");
    nkinfile_open(&f, my_block_read_2, "Hello world!\n", 1, buf);

    while (!nk_feof(&f))
        printf("%c", nk_fgetc(&f));

    printf("Blocks2, test nk_fnext: ");

    nk_fseek(&f, 0);

    for (c = nk_fpeek(&f); c != -1; c = nk_fnext(&f))
        printf("%c", c);

    printf("Blocks2, test nk_fpeek_abs: ");

    nk_fseek(&f, 0);

    for (x = 0; x != 13; ++x)
        printf("%c", nk_fpeek_abs(&f, x));

    printf("..check eof: %d\n", nk_fpeek_abs(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Blocks2, test nk_fpeek_rel: ");

    nk_fseek(&f, 6);

    for (x = -6; x != 7; ++x)
        printf("%c", nk_fpeek_rel(&f, x));

    printf("..check eof: %d\n", nk_fpeek_rel(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Blocks3, test nk_fgetc: ");
    nkinfile_open(&f, my_block_read_2, "Hello world\n", 1, buf);

    while (!nk_feof(&f))
        printf("%c", nk_fgetc(&f));

    printf("Blocks3, test nk_fnext: ");

    nk_fseek(&f, 0);

    for (c = nk_fpeek(&f); c != -1; c = nk_fnext(&f))
        printf("%c", c);

    printf("Blocks3, test nk_fpeek_abs: ");

    nk_fseek(&f, 0);

    for (x = 0; x != 12; ++x)
        printf("%c", nk_fpeek_abs(&f, x));

    printf("..check eof: %d\n", nk_fpeek_abs(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    printf("Blocks3, test nk_fpeek_rel: ");

    nk_fseek(&f, 6);

    for (x = -6; x != 6; ++x)
        printf("%c", nk_fpeek_rel(&f, x));

    printf("..check eof: %d\n", nk_fpeek_rel(&f, x));
    printf("..check currnt: %c\n", nk_fpeek(&f));

    return 0;
}
