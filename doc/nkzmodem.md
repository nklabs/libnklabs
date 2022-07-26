# nkzmodem: ZMODEM protocol

These functions provide a way to transfer files over a serial port using the
classic ZMODEM protocol.  This protocol is convenient because it is
supported in many terminal emulators, meaning that no software has to be
written for the host and there is defacto compatibility with many host
operating systems.

The ZMODEM protocol is used (vs. the other classic serial port protocols),
because it is efficient and can handle binary files (it escapes control
characters which may not make it through the serial port such as Ctrl-Q and
Ctrl-S).

Files: nkzmodem.c nkzmodem.h zmodem.h zmodemr.c zmodemt.c zmutil.c

Functions used: UART functions: nk_uart_read, nk_set_uart_mode, nk_uart_write,
nk_puts.  Flash functions: nk_flash_erase, nk_flash_write.  CLI functions:
nk_scan.

## cmd_zmodem()

This is a command line interface with the following commands:

	zmodem

Receive a file from the host using ZMODEM protocol.  The file is written to
flash memory.  Typically this is used for firmware update.  ZOpenFile (in
nkzmodem.c) parses the file name, and can be used to target different areas
of the flash memory depending on the name.

	zmodem send

Send a small test file to the host using ZMODEM protocol.  This is to verify
that ZMODEM transfer to host is working.

## nk_zsend_file()

```c
void nk_zsend_file(
    const char *name, 
    FILE *(*topen)(const char *name, const char *mode),
    void (*tclose)(FILE *f),
    int (*tgetc)(FILE *f),
    void (*tseek)(FILE *f, size_t offset, int ign),
    int (tsize)(FILE *f)
);
```

Send a file to host over serial port using ZMODEM protocol.  The file name
on the host will be 'name'.  You provide file access functions that match
the stdio ones.  (These functions do not necessarily have to read from a
file).

## nk_zsend_buffer()

```c
void nk_zsend_buffer(const char *name, char *buffer, size_t len);
```

Send a memory buffer to host over serial port using ZMODEM protocol.  The
file name on the host will be 'name'.
