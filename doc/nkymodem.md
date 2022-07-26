# nkymodem: YMODEM protocol

These functions provide a way to transfer files over the console serial port
using the classic YMODEM protocol.  This protocol is convenient because it
is supported in many terminal emulators, meaning that no software has to be
written for the host and there is defacto compatibility with many host
operating systems.

The YMODEM protocol is used (vs.  the other classic serial port protocols),
because it is efficient (supports 1K packets), can handle binary files (this
requires UART-link to be 8-bit clean), and transfers the file name.

## Files

[nkymodem.c](../src/nkymodem.c),
[nkymodem.h](../inc/nkymodem.h),
[nkymodem_config.h](../config/nkymodem_config.h),
[nkymodem_cmd.c](../app/nkymodem_cmd.c)

Functions used: UART functions: nk_uart_read, nk_set_uart_mode, nk_uart_write,
nk_puts.  Flash functions: nk_flash_erase, nk_flash_write.  CLI functions:
nk_scan.


## Configuration options

```c
// Define this to allow 1K packets.  Otherwise only allow 128 byte packets. 
// This could be used to save RAM by reducing the size of the necessary
// receive buffer.
#define NK_YM_ALLOWLONG

// Define this to send NAK instead of C, to indicate to the sender to
// use checksums intead of CRC.
// #define NK_YM_NOCRC

// Comment out to use XMODEM send instead of YMODEM send.  This could be
// used to save some code space if the filename is not needed.
#define YMODEM_SEND
```

## YMODEM vs. XMODEM

X-MODEM:

* Onle a single file per transfer is allowed
* The file name is not transferred
* The file size is not transferred, so the recevied size will be a multiple of 128 bytes

Y-MODEM:

* Multiple files per transfer are allowed
* The file name and exact file size are included in a header for each file

Both protocols support either CRC or checksums for packets.  The receiver
indicates that he wants CRCs by transmitting 'C' to request the first
packet.  The receiver indicates that he watns checksums by transmitting NAK
instead.

## cmd_ymodem()

This is a command line interface with the following commands:

	ymodem

Receive a file from the host using YMODEM protocol.  The file is written to
flash memory.  Typically this is used for firmware update. 
ymodem_recv_file_open (in nkymodem_cmd.c) parses the file name, and can be
used to target different areas of the flash memory depending on the name.

	ymodem send

Send a small test file to the host using YMODEM protocol.  This is to verify
that YMODEM transfer to host is working.

## nk_ysend_file()

```c
void nk_ysend_file(
    unsigned char *packet_buffer,
    nkinfile_t *file,
    const char *name, 
    uint32_t size
);
```

Send a file to host over serial port using YMODEM protocol.  The file name
on the host will be 'name'.  You provide an open nkinfile_t from which
nk_ysend_file will read the data to send.  'size' has the file size.

You also provide an address to a packet buffer, whose size must be
NK_YM_BUFFER_SIZE.

## nk_ysend_buffer()

```c
void nk_ysend_buffer(unsigned char *packet_buffer, const char *name, void *buffer, size_t len);
```

Send a memory buffer to host over serial port using YMODEM protocol.  The
file name on the host will be 'name'.

You also provide an address to a packet buffer, whose size must be
NK_YM_BUFFER_SIZE.

## nk_yrecv()

```c
int nk_yrecv(unsigned char *packet_buffer);
```

You provide an address to a packet buffer, whose size must be
NK_YM_BUFFER_SIZE.

Receive a file from console serial port using YMODEM protocol.  The return
value indicates the transfer status:

	YMODEM_RECV_DONE: file was recevied with no error

	YMODEM_RECV_REMOTE_CANCEL: host-side canceled the transfer

	YMODEM_RECV_OPEN_CANCEL: receove-side couldn't open file

nk_yrecv requires that you provide the following functions to write the file:

```c
int ymodem_recv_file_open(char *name);

void ymodem_recv_file_write(unsigned char *buffer, int len);

void ymodem_recv_file_close();

void ymodem_recv_file_cancel();

void ymodem_recv_all_done();

```

ymodem_recv_file_open is called when the file should be opened.  'name'
contains a NUL-terminated file-name.  ymodem_recv_file_open should return 0
if there were no errors.

ymodem_recv_file_write is called to append some recevied data to the file.

ymodem_recv_file_close is called after all data has been received.

ymodem_recv_file_cancel is called instead of ymodem_recv_file_close if the
transfer was canceled for some reason.

ymodem_recv_all_done is called after the protocol is finished, when it is
safe to print to the console.
