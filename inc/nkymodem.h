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

// Ymodem uses nk_putc, nk_getc and nk_uart_read

// ymodem_send return status

enum {
    YMODEM_SEND_STATUS_MORE = 1,
    YMODEM_SEND_STATUS_DONE = 2,
    YMODEM_SEND_STATUS_CANCEL = 3
};

// Transmit a file

void nk_ysend_file(
    const char *name, 
    void *(*topen)(const char *name, const char *mode),
    void (*tclose)(void *f),
    int (*tgetc)(void *f),
    int (tsize)(void *f)
);

// Transmit a memory buffer as a file
//  This calls nk_ysend_file with functions to set send the buffer

void nk_ysend_buffer(const char *name, char *buffer, size_t len);

// Receive file handler used by nk_yrecv.  This is provided
// in nkymodem.c

// Called by ymodem_rcv to open receive file

int ymodem_recv_file_open(char *name);

// Called by ymodem_rcv to write data to file

void ymodem_recv_file_write(unsigned char *buffer, int len);

// Called by ymodem_rcv to close receive file after transfer complete

void ymodem_recv_file_close();

// Called by ymodem_rcv to close receive file for case of canceled transfer

void ymodem_recv_file_cancel();

// Prepare to receive

void ymodem_recv_init();

// ymodem_yrecv return status

enum {
    YMODEM_RECV_DONE = 1,
    YMODEM_RECV_REMOTE_CANCEL = 2,
    YMODEM_RECV_OPEN_CANCEL = 3,
    YMODEM_RECV_SYNC_CANCEL = 4,
    YMODEM_RECV_MORE = 5,
    YMODEM_RECV_PURGE = 6
};

// Processed some received data

int ymodem_rcv(unsigned char *rcvbuf, int len);

// Receive and process a file
//  This calls ymodem_recv_init and ymodem_rcv.

void nk_yrecv();

// Print some receive status for debugging
void debug_rcv_status();

/* X/Y-MODEM serial file transfer protocol notes:

X-MODEM vs. Y-MODEM:

  X-MODEM Does not send filename, and there is only one file per transfer.

  Since there is no filename, size must be a mutliple of 128 bytes.  For text files,
  end of file is indicated by Ctrl-Z in last block.

  X-MODEM Starts with packet sequence number 1.

  Y-modem file header starts with sequence number 0.
  
  Y-modem file header starts with NUL if there are no more files in the transfer.

Why prefer Y-MODEM?
  1. It's simple and tiny (not a complex windowing protocol like zmodem)
  2. It's efficient, uses 1K packets (with -k option)
  3. It can handle binary data
  4. Exact transfer length is known (sent along with file name)
  5. Uses CRC16 for data integrity checking

Maybe Y-MODEM can handle Xon/Xoff handshaking, but only the sender should
have it enabled.  Y-MODEM receiver never sends XON/XOFF (but sender does! 
XOFF could occur in binary data or checksums, so receive side should not
handle XON/XOFF on input).

But in general it's better to provide a 1K buffer so that it can receive a
full packet without pushback.  Pauses between packets are no problem.

One bad thing is that there is no way for the receiver to indicate to the
sender that it can only handle 128-byte packets- but by default "sz" uses 128
byte packets if CRC is disabled, as follows:

    sz/sb command:

    If receive side sends 'C' (indicating that it wants CRC16 packets):
    sz              Tries zmodem, then falls back to ymodem with 1K option
    sz --ymodem     Uses ymodem with 1K packets
    sz --ymodem -k  Uses ymodem with 1K packets
    sb              Uses ymodem with 1K packets
    sx              Uses xmodem with 128 byte packets

    If receive side sends 'NAK' (indicating that it wants checksum packets):
    sz              Tries zmodem, then falls back to ymodem with 1K packets
    sz --ymodem     Uses ymodem 128 byte packets only
    sz --ymodem -k  Uses ymodem with 1K packets
    sb              Uses ymodem with 128 byte packets
    sx              Uses xmodem with 128 byte packets

    "sz" will try zmodem first, then fall back to Ymodem when it sees the 'C'. 
    But note that YMODEM receiver has to be prepared to handle the initial
    zmodem header as junk.

    "sz --ymodem" forces it to use Y-MODEM mode.

Sender uses a 1 minute timeout and the protocol is generally receiver driven.

Sender can send two CANs (Ctrl-Xs) to cancel a transfer:

	Valid when receiver is waiting for a packet.

Receiver can send CAN to cancel a transfer:

	Valid when sender is waiting for ACK.

Receive should handle any mixture of 128 or 1024 byte packets.  Transmitter
should not change packet sizes except after an ACK.  In other words, it
can't change the packet size on a resend due to a NAK.

Receiver has a 10 second timeout- it generally sends NAK on timeout, but instead
sends 'C' before the file header or first packet of file to indicate CRC16 packets.

Receiver should expect the next sequence number or a repeat of the same sequence number.
If invalid sequence number received, send CAN and abort- there is no way to recover from
a sequence number issue.

When sending a NAK, purge receiver first.  Purge means wait until sender has
stopped sending for at least 1 second.

Receiver sends 'C' to indicate its desire for CRC16 packets.  If it sends
NAK instead, sender uses checksum packets.  

Sender                           Receiver
------				---------

				10-sec timeout

                                 C  <- Start, use CRC-16   - OR -     NAK <- Start, use checksum

SOH 00 FF Data[128] CRC CRC <- NUL terminated file name in Data[128]
                                 ACK (or CAN if can't open file)

				 1-sec timeout

                                 C (to ask for file contents)

Name: foo.c NUL length SPC mod-date SPC mode SPC serno NUL
  length in ascii decimal
  mod-date in octal seconds from Jan 1 1970
  mode octal UNIX mode
  serno octal (0 if no serial number)


STX 01 FE Data[1024] CRC CRC
                                 ACK

STX 02 FD Data[1024] CRC CRC
                                 ACK

SOH 03 FC Data[128] CRC CRC
                                 ACK
EOT
				 NAK
EOT
                                 ACK

				1-sec timeout

				 C (ask for next file)

SOH 00 FF NUL[128] CRC CRC  (all NULs indicate no more files) 
                                 ACK


Here is an example packet with good CRC:

0000: 01 00 ff 68 65 6c 6c 6f  00 31 35 20 31 34 31 34  ...hello.15 1414
0010: 31 30 32 31 33 30 35 20  31 30 30 36 36 34 20 30  1021305 100664 0
0020: 20 31 20 31 35 00 00 00  00 00 00 00 00 00 00 00   1 15...........
0030: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0040: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0050: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0060: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0070: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0080: 00 00 01 d5 7e                                    ....~           

CRC is 16-bit big endian.

Here is an example with good checksum:

0000: 01 00 ff 68 65 6c 6c 6f  00 31 32 38 20 31 34 31  ...hello.128 141
0010: 34 31 30 34 32 37 32 32  20 31 30 30 36 36 34 20  41042722 100664 
0020: 30 20 31 20 31 32 38 00  00 00 00 00 00 00 00 00  0 1 128.........
0030: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0040: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0050: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0060: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0070: 00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................
0080: 00 00 01 a9

Characters:

0x01  SOH
0x02  STX
0x04  EOT
0x06  ACK
0x15  NAK
0x18  CAN
0x43  C

CRC function:

    unsigned short updcrc(c, crc)
    register c;
    register unsigned crc;
    {
            register count;

            for (count=8; --count>=0;) {
                    if (crc & 0x8000) {
                            crc <<= 1;
                            crc += (((c<<=1) & 0400)  !=  0);
                            crc ^= 0x1021;
                    }
                    else {
                            crc <<= 1;
                            crc += (((c<<=1) & 0400)  !=  0);
                    }
            }
            return crc;
    }

CRC and checksum cover only the data part of the packet, not head SOH/STX or sequence number.

Checksum is just byte by byte add (no carry wrapping like TCP/IP).

*/
