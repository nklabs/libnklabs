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

// X/Y-modem protocol

#include <string.h>
#include "nkprintf.h"
#include "nkcrclib.h"
#include "nkcli.h"
#include "nkchecked.h"
#include "nksched.h"
#include "nkymodem.h"

int timeout_tid;

#define NK_YM_SOH 0x01
#define NK_YM_STX 0x02
#define NK_YM_EOT 0x04
#define NK_YM_ACK 0x06
#define NK_YM_NAK 0x15
#define NK_YM_CAN 0x18

// Packet lengths including SOH/STX, sequencen number and CRC
#define NK_YM_SHORT_PACKET_LEN 133u
#define NK_YM_LONG_PACKET_LEN 1029u

// Index into packet_buf for saving multiple packets for debugging

static size_t last_idx;

// Receive buffer

#ifdef NK_YM_ALLOWLONG
static unsigned char packet_buf[NK_YM_LONG_PACKET_LEN]; // Enough for STX 01 FE Data[1024] CRC CRC
#else
static unsigned char packet_buf[NK_YM_SHORT_PACKET_LEN]; // Enough for SOH 01 FE Data[128] CRC CRC
#endif

#ifdef NK_YM_DEBUG_LOG_SIZE

static void debug_logit(uint8_t state, uint8_t c);
static void debug_clrit();
static void debug_log_show();

#define NK_YM_DEBUG_LOGIT(state, c) debug_logit(state, c)
#define NK_YM_DEBUG_CLRIT() debug_clrit()
#define NK_YM_DEBUG_LOG_SHOW() debug_log_show()

#else

#define NK_YM_DEBUG_LOGIT(state, c)
#define NK_YM_DEBUG_CLRIT()
#define NK_YM_DEBUG_LOG_SHOW()

#endif

// Optional debug log

#ifdef NK_YM_DEBUG_LOG_SIZE

static struct debug_log_entry
{
    uint8_t state;
    uint8_t c;
} debug_log[NK_YM_DEBUG_LOG_SIZE];

static int debug_log_idx;

static void debug_logit(uint8_t state, uint8_t c)
{
    if (debug_log_idx != NK_YM_DEBUG_LOG_SIZE)
    {
        debug_log[debug_log_idx].state = state;
        debug_log[debug_log_idx].c = c;
        ++debug_log_idx;
    }
}

static void debug_clrit()
{
    debug_log_idx = 0;
}

static void debug_log_show()
{
    int x;
    if (last_idx) {
        nk_byte_hex_dump(packet_buf, 0, 0, last_idx);
        
    }
    for (x = 0; x != debug_log_idx; ++x)
    {
        nk_printf("State = %x, Char = %x\n", debug_log[x].state, debug_log[x].c);
    }
}

#endif

// Ymodem receive state

static char ymodem_opened;
static char ymodem_got_eot; // Set if we got first EOT
static char ymodem_purge; // Set if we're purging
static char ymodem_first;
static char ymodem_can;
static char ymodem_to;
static char ymodem_xmodem; // Set for Xmodem mode: anonymous file detected
static unsigned char ymodem_nocrc = NK_YM_NOCRC;
static uint8_t ymodem_old_seq; // Seq. number of previous packet
static uint8_t ymodem_seq; // Next expected sequence number
static uint32_t ymodem_file_size; // Exact size of file or 0xffffffff

// Ymodem send state

static uint8_t ymodem_send_eot;
static uint8_t ymodem_send_fin;
static uint8_t ymodem_send_seq;
static uint8_t ymodem_send_hdr;
static uint16_t ymodem_send_block_size;

static void ymodem_setup_header()
{
    packet_buf[0] = NK_YM_SOH;
    packet_buf[1] = ymodem_send_seq;
    packet_buf[2] = (uint8_t)~ymodem_send_seq;
#ifdef NK_YM_YMODEM_SEND
    unsigned short crc = nk_crc16be_check(packet_buf + 3, 128);
    packet_buf[131] = (uint8_t)(crc >> 8);
    packet_buf[132] = (uint8_t)crc;
    ymodem_send_block_size = 133;
#else
    uint8_t cksum = 0;
    int x;
    for (x = 0; x != 128; ++x)
        cksum += packet_buf[3 + x];
    packet_buf[131] = cksum;
    ymodem_send_block_size = 132;
#endif
}

#ifdef NK_YM_YMODEM_SEND
static void ymodem_prepare_fin()
{
    memset(packet_buf + 3, 0, 128);
    ymodem_send_seq = 0;
    ymodem_setup_header();
}
#endif

static int ymodem_prepare_next(int (*tgetc)(void *file), void *file)
{
    int x;
    // Load next block
    for (x = 0; x != 128; ++x)
    {
        int c = tgetc(file);
        if (c == -1)
            break;
        packet_buf[3 + x] = (unsigned char)c;
    }
    if (x == 0)
        return 0; // Nothing more to send

    while (x != 128)
    {
        packet_buf[3 + x++] = 0;
    }
    ymodem_setup_header();
    return 1; // Next block ready
}

static void ymodem_send_block()
{
    int x;
    for (x = 0; x != ymodem_send_block_size; ++x)
        nk_putc(packet_buf[x]);
}

// Send a file event-
// Call whenever a character is received

static int nk_ysend_evt(int (*tgetc)(void *), void *file, unsigned char c)
{
    int status = YMODEM_SEND_STATUS_MORE;
    NK_YM_DEBUG_LOGIT(
        (uint8_t)((ymodem_send_eot << 0) |
        (ymodem_send_fin << 1)),
        c);
    if (c == 'C' || c == NK_YM_NAK)
    {
        if (ymodem_send_eot)
        {
            nk_putc(NK_YM_EOT);
            NK_YM_DEBUG_LOGIT(4, 0x40);
        }
        else
        {
            // Retransmit
            ymodem_send_block();
            NK_YM_DEBUG_LOGIT(4, 0x41);
        }
    }
    else if (c == NK_YM_ACK)
    {
        if (ymodem_send_eot)
        {
            ymodem_send_eot = 0;
#ifdef NK_YM_YMODEM_SEND
            ymodem_prepare_fin();
            ymodem_send_fin = 1;
            ymodem_send_hdr = 1;
            // We don't send it here, other end if going to send C after the ACK
            NK_YM_DEBUG_LOGIT(4, 0x42);
#else
            status = YMODEM_SEND_STATUS_DONE;
            NK_YM_DEBUG_LOGIT(4, 0x43);
#endif
        }
        else if (ymodem_send_fin)
        {
            status = YMODEM_SEND_STATUS_DONE;
            NK_YM_DEBUG_LOGIT(4, 0x44);
        }
        else
        {
            ++ymodem_send_seq;
            // Transmit next
            if (ymodem_prepare_next(tgetc, file))
            {
                // Transmit
                // We don't send first data after ACK of header, instead we wait for other end to request it with C
                if (ymodem_send_hdr)
                    ymodem_send_hdr = 0;
                else
                    ymodem_send_block();
                NK_YM_DEBUG_LOGIT(4, 0x45);
            }
            else
            {
                // Send EOT, we're done
                ymodem_send_eot = 1;
                // Zero length file right after ACK for header: wait for other end to request it with C
                if (ymodem_send_hdr)
                    ymodem_send_hdr = 0;
                else
                    nk_putc(NK_YM_EOT);
                NK_YM_DEBUG_LOGIT(4, 0x46);
            }
        }
    }
    else if (c == NK_YM_CAN)
    {
        // Cancel
        status = YMODEM_SEND_STATUS_CANCEL;
        NK_YM_DEBUG_LOGIT(4, 0x47);
    }
    else
    {
        // Junk
        // Just ignore it
        NK_YM_DEBUG_LOGIT(4, 0x48);
    }
    return status;
}

// This is the UART callback function

static void *ysend_file;
static int (*ysend_tgetc)(void *f);
static void (*ysend_tclose)(void *f);
// static int ymodem_async;

static void ymodem_send_task(void *data)
{
    int sta = YMODEM_SEND_STATUS_MORE;
    (void)data;
    do {
        int c = nk_getc();
        if (c != -1)
            sta = nk_ysend_evt(ysend_tgetc, ysend_file, (unsigned char)c);
        else
            break;
    } while (sta == YMODEM_SEND_STATUS_MORE);

    if (sta == YMODEM_SEND_STATUS_MORE)
    {
        nk_set_uart_callback(nk_cli_tid, ymodem_send_task, NULL);
    }
    else
    {

        // Restore UART to console mode
        // Re-enable background messages
        nk_set_uart_mode(0);
        // async_log_set(ymodem_async);

        // Delay to allow other end to restore terminal
        nk_udelay(1000000);
        nk_printf("\n");

        ysend_tclose(ysend_file);

        if (sta == YMODEM_SEND_STATUS_CANCEL)
        {
            nk_printf("Transfer canceled.\n");
        }
        else if (sta == YMODEM_SEND_STATUS_DONE)
        {
            nk_printf("Transfer complete.\n");
        }
        else
        {
            nk_printf("Unknown status\n");
        }

        // Re-enable CLI on UART
        nk_cli_enable();
    }
}

// Send a file using ymodem protocol
// This disbles the CLI until the transfer is complete or canceled

void nk_ysend_file(
    const char *name, 
    void *(*topen)(const char *name, const char *mode),
    void (*tclose)(void *f),
    int (*tgetc)(void *f),
    int (tsize)(void *f)
) {
    ysend_tgetc = tgetc;
    ysend_tclose = tclose;
    ysend_file = topen(name, "r");
    
    if (!ysend_file)
    {
        nk_printf("Couldn't open file\n");
        return;
    }

    nk_printf("Start YMODEM receive transfer...\n");
    nk_printf("Hit Ctrl-X to cancel.\n");

    // Disable CLI
    nk_cli_disable();
    // Change UART to 8-bit clear mode
    // Disable background messages
    // int async;
    // ymodem_async = async_log_set(0);
    nk_set_uart_mode(1);

    NK_YM_DEBUG_CLRIT();
    ymodem_send_eot = 0;
    ymodem_send_fin = 0;

    // First block: file name
#ifdef NK_YM_YMODEM_SEND
    ymodem_send_seq = 0;
    memset(packet_buf + 3, 0, 128);
    nk_snprintf((char *)packet_buf + 3, 125, "%s%c%d", name, 0, tsize(ysend_file));
    ymodem_setup_header();
    ymodem_send_hdr = 1;
#else
    ymodem_send_seq = 1;
    ymodem_send_hdr = 0;
    if (!ymodem_prepare_next(tgetc, file))
    {
        ymodem_send_eot = 1;
    }
#endif

    nk_set_uart_callback(nk_cli_tid, ymodem_send_task, NULL);
}

// Funcitons for transferring a memory buffer as a file

static char *sdata;
static size_t sdata_offset;
static size_t sdata_size;
static int sdata_eof;

static void *tzopen(const char *name, const char *mode)
{
    static char fake[] = "fak";
    (void)name;
    (void)mode;
    sdata_offset = 0;
    sdata_eof = 0;
    return (void *)fake;
}

static void tzclose(void *f)
{
    (void)f;
}

static int tzgetc(void *file)
{
    (void)file;
    if (sdata_offset == sdata_size) {
        sdata_eof = 1;
        return -1;
    } else {
        unsigned char c;
        c = sdata[sdata_offset];
        sdata_offset++;
        return c;
    }
}

static int tzsize(void *f)
{
    (void)f;
    return (int)sdata_size;
}

// Transmit a buffer as a file

void nk_ysend_buffer(const char *name, char *buffer, size_t len)
{
    sdata = buffer;
    sdata_offset = 0;
    sdata_size = len;
    sdata_eof = 0;
    return nk_ysend_file(name, tzopen, tzclose, tzgetc, tzsize);
}

// Ymodem receive protocol

// Prepare to receive next file

void ymodem_recv_init()
{
    ymodem_opened = 0;
    ymodem_got_eot = 0;
    ymodem_purge = 0;
    ymodem_seq = 0;
    ymodem_first = 0;
    ymodem_can = 0;
    ymodem_to = 10;
    ymodem_xmodem = 0;
    // Poke other end: ask for CRCs
    if (ymodem_nocrc)
    {
        nk_putc(NK_YM_NAK);
        NK_YM_DEBUG_LOGIT(0, NK_YM_NAK);
    }
    else
    {
        nk_putc('C');
        NK_YM_DEBUG_LOGIT(0, 'C');
    }
}

// Checksum or CRC validation

static int crc_count; // Count of CRC packets
static int cksum_count; // Count of checksum packets

static int ymcrc_check(uint8_t *buf, size_t len)
{
    if (ymodem_nocrc)
    {
        size_t x;
        uint8_t cksum = 0;
        for (x = 0; x != len - 2; ++x)
            cksum = (uint8_t)(cksum + buf[x]);
        if (cksum == buf[x])
        {
            ++cksum_count;
            return 1;
        }
        else
            return 0;
    }
    else
    {
        unsigned short crc = nk_crc16be_check(buf, len);
        if (!crc)
            ++crc_count;
        return !crc;
    }
}

// Handle some receive data

static int long_count; // Count of received long packets- for debugging

int ymodem_rcv(unsigned char *rcvbuf, size_t len)
{
    int status = YMODEM_RECV_MORE;

    if (ymodem_purge)
    {
        if (len)
        {
            // We're purging, just ignore this data
            status = YMODEM_RECV_PURGE;
        }
        else
        {
            // Timeout- ask for resend
            if (ymodem_nocrc)
            {
                nk_putc(NK_YM_NAK); // Use C instead of NAK here to ensure CRC mode
                NK_YM_DEBUG_LOGIT(1, NK_YM_NAK);
            }
            else
            {
                nk_putc('C'); // Use C instead of NAK here to ensure CRC mode
                NK_YM_DEBUG_LOGIT(1, 'C');
            }
            ymodem_purge = 0;
        }
    }
    else if (!ymodem_opened)
    {
        // Not opened: we are waiting for SOH packet with file name or empty SOH packet indicating transfer done
        if (!len)
        {
            if (ymodem_to)
            {
                --ymodem_to;
            }
            else
            {
                // Timeout, retry
                ymodem_recv_init();
            }
        }
        else if (len == 1 && rcvbuf[0] == NK_YM_CAN)
        {
            // Transmit side is canceling
            if (ymodem_can)
            {
                status = YMODEM_RECV_REMOTE_CANCEL;
            }
            else
            {
                // Got first CAN
                ymodem_can = 1;
                ymodem_to = 10;
            }
        }
        else if ((len == NK_YM_SHORT_PACKET_LEN-ymodem_nocrc || len == NK_YM_LONG_PACKET_LEN-ymodem_nocrc) && rcvbuf[0] == NK_YM_SOH && rcvbuf[1] == 0 && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2])  && ymcrc_check(rcvbuf+3, NK_YM_SHORT_PACKET_LEN-3))
        {
            // Ymodem file header received
            ymodem_can = 0;
            ymodem_to = 0;
            // Valid open file packet
            if (rcvbuf[3] == 0)
            {
                // No more files
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(2, NK_YM_ACK);
                status = YMODEM_RECV_DONE;
            }
            else
            {
                size_t name_len = strlen((char *)rcvbuf + 3);
                // Get exact file size
                if (rcvbuf[3 + name_len + 1])
                {
                    ymodem_file_size = (uint32_t)atoi((char *)(rcvbuf + 3 + name_len + 1));
                }
                else
                {
                    ymodem_file_size = 0xffffffff;
                }
                if (!ymodem_recv_file_open((char *)rcvbuf+3))
                {
                    nk_putc(NK_YM_ACK);
                    NK_YM_DEBUG_LOGIT(3, NK_YM_ACK);
                    ymodem_old_seq = 0;
                    ymodem_seq = 1;
                    // We are going to send 'C' next: but do it after timeout
                    ymodem_opened = 1;
                    ymodem_first = 1;
                }
                else
                {
                    nk_putc(NK_YM_CAN);
                    NK_YM_DEBUG_LOGIT(3, NK_YM_CAN);
                    nk_putc(NK_YM_CAN);
                    NK_YM_DEBUG_LOGIT(3, NK_YM_CAN);
                    status = YMODEM_RECV_OPEN_CANCEL;
                }
            }
        }
        else if ((len == NK_YM_SHORT_PACKET_LEN-ymodem_nocrc || len == NK_YM_LONG_PACKET_LEN-ymodem_nocrc) && rcvbuf[0] == NK_YM_SOH && rcvbuf[1] == 1 && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2])  && ymcrc_check(rcvbuf+3, NK_YM_SHORT_PACKET_LEN-3))
        {
            // Xmodem first packet of anonymous file received
            ymodem_can = 0;
            ymodem_to = 0;
            // Valid open file packet
            if (rcvbuf[3] == 0)
            {
                // No more files
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(4, NK_YM_ACK);
                status = YMODEM_RECV_DONE;
            }
            else
            {
                ymodem_file_size = 0xffffffff;
                if (!ymodem_recv_file_open("anonymous\0"))
                {
                    nk_putc(NK_YM_ACK);
                    NK_YM_DEBUG_LOGIT(5, NK_YM_ACK);
                    ymodem_old_seq = 1;
                    ymodem_seq = 2;
                    // We are going to send 'C' next: but do it after timeout
                    ymodem_opened = 1;
                    ymodem_xmodem = 1; // Xmodem mode
                    uint32_t this_size = 128;
                    if (this_size > ymodem_file_size)
                    {
                        this_size = ymodem_file_size;
                        ymodem_file_size = 0;
                    }
                    else
                    {
                        ymodem_file_size -= this_size;
                    }
                    if (this_size)
                        ymodem_recv_file_write(rcvbuf + 3, this_size);
                    ymodem_first = 0;
                }
                else
                {
                    nk_putc(NK_YM_CAN);
                    NK_YM_DEBUG_LOGIT(6, NK_YM_CAN);
                    nk_putc(NK_YM_CAN);
                    NK_YM_DEBUG_LOGIT(6, NK_YM_CAN);
                    status = YMODEM_RECV_OPEN_CANCEL;
                }
            }
        }
        else
        {
            // We received junk
            ymodem_can = 0;
            status = YMODEM_RECV_PURGE;
            ymodem_purge = 1;
            ymodem_to = 0;
        }
    }
    else
    {
        // File is opened: we are waiting for SOH or STX packet with data
        // Or EOT indicating no more data
        if (!len)
        {
            // Timeout
            ymodem_got_eot = 0;
            ymodem_can = 0;
            if (ymodem_first)
            {
                if (ymodem_nocrc)
                {
                    nk_putc(NK_YM_NAK);
                    NK_YM_DEBUG_LOGIT(7, NK_YM_NAK);
                }
                else
                {
                    nk_putc('C');
                    NK_YM_DEBUG_LOGIT(7, 'C');
                }
            }
            else
            {
                nk_putc(NK_YM_NAK);
                NK_YM_DEBUG_LOGIT(7, NK_YM_NAK);
            }
        }
        else if (len == 1 && rcvbuf[0] == NK_YM_CAN)
        {
            ymodem_got_eot = 0;
            if (ymodem_can)
            {
                // Transmit side is canceling
                ymodem_recv_file_cancel();
                status = YMODEM_RECV_REMOTE_CANCEL;
            }
            else
            {
                ymodem_can = 1;
            }
        }
        else if (len == NK_YM_SHORT_PACKET_LEN-ymodem_nocrc && rcvbuf[0] == NK_YM_SOH && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2]) && ymcrc_check(rcvbuf+3, NK_YM_SHORT_PACKET_LEN-3))
        {
            ymodem_got_eot = 0;
            ymodem_can = 0;
            if (rcvbuf[1] == ymodem_old_seq)
            {
                // Missed ACK- ignore data and resend ACK
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(8, NK_YM_ACK);
            }
            else if (rcvbuf[1] != ymodem_seq)
            {
                // Out of sync
                nk_putc(NK_YM_CAN);
                NK_YM_DEBUG_LOGIT(8, NK_YM_CAN);
                nk_putc(NK_YM_CAN);
                NK_YM_DEBUG_LOGIT(8, NK_YM_CAN);
                ymodem_recv_file_cancel();
                status = YMODEM_RECV_SYNC_CANCEL;
            }
            else
            {
                // Good receive data
                uint32_t this_size = 128;
                if (this_size > ymodem_file_size)
                {
                    this_size = ymodem_file_size;
                    ymodem_file_size = 0;
                }
                else
                {
                    ymodem_file_size -= this_size;
                }
                if (this_size)
                    ymodem_recv_file_write(rcvbuf + 3, this_size);
                ymodem_first = 0;
                ymodem_old_seq = ymodem_seq++;
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(9, NK_YM_ACK);
            }
        }
#ifdef NK_YM_ALLOWLONG
        else if (len == NK_YM_LONG_PACKET_LEN-ymodem_nocrc && rcvbuf[0] == NK_YM_STX && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2]) && ymcrc_check(rcvbuf+3, NK_YM_LONG_PACKET_LEN-3))
        {
            ymodem_got_eot = 0;
            ymodem_can = 0;
            if (rcvbuf[1] == ymodem_old_seq)
            {
                // Missed ACK- ignore data and resend ACK
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(10, NK_YM_ACK);
            }
            else if (rcvbuf[1] != ymodem_seq)
            {
                // Out of sync
                nk_putc(NK_YM_CAN);
                NK_YM_DEBUG_LOGIT(10, NK_YM_CAN);
                nk_putc(NK_YM_CAN);
                NK_YM_DEBUG_LOGIT(10, NK_YM_CAN);
                ymodem_recv_file_cancel();
                status = YMODEM_RECV_SYNC_CANCEL;
            }
            else
            {
                // Good receive data
                uint32_t this_size = 1024;
                if (this_size > ymodem_file_size)
                {
                    this_size = ymodem_file_size;
                    ymodem_file_size = 0;
                }
                else
                {
                    ymodem_file_size -= this_size;
                }
                if (this_size)
                    ymodem_recv_file_write(rcvbuf + 3, this_size);
                ymodem_first = 0;
                ymodem_old_seq = ymodem_seq++;
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(10, NK_YM_ACK);
                ++long_count;
            }
        }
#endif
        else if (len == 1 && rcvbuf[0] == NK_YM_EOT)
        {
            ymodem_can = 0;
            if (ymodem_xmodem)
            {
                ymodem_recv_file_close();
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(11, NK_YM_ACK);
                status = YMODEM_RECV_DONE;
            }
            else if (ymodem_got_eot)
            {
                ymodem_recv_file_close();
                nk_putc(NK_YM_ACK);
                NK_YM_DEBUG_LOGIT(12, NK_YM_ACK);
                ymodem_recv_init();
                ymodem_to = 0;
                
            }
            else
            {
                ymodem_got_eot = 1; // We want to see EOT twice
                nk_putc(NK_YM_NAK);
                NK_YM_DEBUG_LOGIT(13, NK_YM_NAK);
            }
        }
        else
        {
            ymodem_got_eot = 0;
            ymodem_can = 0;
            ymodem_purge = 1;
            status = YMODEM_RECV_PURGE;
        }
    }
    return status;
}

static int laststatus; // Final ymodem receive status- for "ymodem show" command

static int yrecv_mode = 0;
static size_t yrecv_len = 0;

char TIMEOUT[] = "timeout!";

extern nk_checked_t ymodem_file;

static int process_full_outer()
{
    int sta = ymodem_rcv(packet_buf + last_idx, yrecv_len);
#ifdef NK_YM_PROTOLOG
    last_idx += yrecv_len;
#endif
    yrecv_len = 0;
    if (sta == YMODEM_RECV_PURGE)
    {
        yrecv_mode = 3;
        return 0;
    }
    else if (sta == YMODEM_RECV_MORE)
    {
        yrecv_mode = 0;
        return 0;
    }
    else
    {
        // We're done
        laststatus = sta;

        // Restore NL->CR-LF mode
        nk_set_uart_mode(0);

        // Time for other end to restore tty
        nk_udelay(1000000);

        // Re-enable background messages
        // async_log_set(async);

        nk_printf("\n");

        // Print message

        switch (sta) {
            case YMODEM_RECV_DONE: {
                nk_printf("Transfer complete: %ld bytes\n", ymodem_file.size);
                ymodem_recv_all_done();
                break;
            } case YMODEM_RECV_REMOTE_CANCEL: {
                nk_printf("Canceled after %ld bytes\n", ymodem_file.size);
                break;
            } case YMODEM_RECV_OPEN_CANCEL: {
                nk_printf("Canceled after %ld bytes (couldn't open file)\n", ymodem_file.size);
                break;
            } default: {
                nk_printf("YMODEM error code %d after %ld bytes\n", sta, ymodem_file.size);
                break;
            }
        }

        // Re-enable CLI on UART
        nk_cli_enable();
        return 1;
    }
}

// Buffer full packet, then call ymodem_rcv

size_t max_len;

static void ymodem_recv_task(void *data)
{
    if (data == TIMEOUT)
    {
        NK_YM_DEBUG_LOGIT(20, 0);
        yrecv_len = 0;
        if (process_full_outer())
            return;
    }
    else
    {
        for (;;)
        {
            int gotit = 0;
            int c = nk_getc();
            if (c != -1)
            {
                packet_buf[last_idx + yrecv_len] = (unsigned char)c;
                ++yrecv_len;
                if (yrecv_len > max_len)
                    max_len = yrecv_len;
                if (yrecv_mode == 0) // First character
                {
                    if (c == NK_YM_SOH)
                    {
                        // Get 132 more
                        yrecv_mode = 1;
                    }
#ifdef NK_YM_ALLOWLONG
                    else if (c == NK_YM_STX)
                    {
                        // Get 1028 more
                        yrecv_mode = 2;
                    }
#endif
                    else if (c == NK_YM_CAN || c == NK_YM_EOT)
                    {
                        gotit = 1;
                    }
                    else
                    {
                        // Ignore it
                        yrecv_len = 0;
                    }
                }
                else if (yrecv_mode == 1) // Short packet
                {
                    if (yrecv_len == NK_YM_SHORT_PACKET_LEN - ymodem_nocrc)
                    {
                        gotit = 1;
                        yrecv_mode = 0;
                    }
                }
                else if (yrecv_mode == 2) // Long packet
                {
                    if (yrecv_len == NK_YM_LONG_PACKET_LEN - ymodem_nocrc)
                    {
                        yrecv_mode = 0;
                        gotit = 1;
                        ++long_count;
                    }
                }
                else if (yrecv_mode == 3) // Purging
                {
                    yrecv_len = 0;
                }

                if (gotit) // We have a complete packet
                {
                    gotit = 0;
                    if (process_full_outer())
                    {
                        nk_unsched(timeout_tid);
                        return;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    // Get more data
    nk_set_uart_callback(nk_cli_tid, ymodem_recv_task, NULL);
    // Restart timeout
    nk_sched(timeout_tid, ymodem_recv_task, TIMEOUT, 1000, "YMODEM timeout");
}

// Receive and process a file

void nk_yrecv()
{
    nk_printf("Start YMODEM send transfer...\r\n");
    nk_printf("Hit Ctrl-X twice to cancel.\r\n");

    // Disable CLI
    nk_cli_disable();

    // Disable background messages
    // ymodem_async = async_log_set(0);

    // Disable NL->CR-LF mode
    nk_set_uart_mode(1);

    last_idx = 0;
    long_count = 0;
    cksum_count = 0;
    crc_count = 0;
    NK_YM_DEBUG_CLRIT();
    ymodem_recv_init();

    if (!timeout_tid)
        timeout_tid = nk_alloc_tid();

    nk_set_uart_callback(nk_cli_tid, ymodem_recv_task, NULL);
    nk_sched(timeout_tid, ymodem_recv_task, TIMEOUT, 1000, "YMODEM timeout");
}

void debug_rcv_status()
{
    nk_printf("status = %d\n", laststatus);
    nk_printf("long_count = %d\n", long_count);
    nk_printf("crc_count = %d\n", crc_count);
    nk_printf("max_len = %zd\n", max_len);
    nk_printf("cksum_count = %d\n", cksum_count);
#ifdef NK_YM_DEBUG_LOG_SIZE
    debug_log_show();
    nk_printf("%p %d\n", debug_log_show, debug_log_idx);
#endif
}
