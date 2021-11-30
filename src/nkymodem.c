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

#include "nkprintf.h"
#include "nkcrclib.h"
#include "nkymodem.h"

// Options:

// Keep all received data in packet_buf for debugging.  Received data is printed with "ymodem show"
// #define PROTOLOG 1

// Allow 1K packets.  When this is disabled, we also set ymodem_nocrc because "sz" command will use 1K
// packets if it detects that receive side can accept packets with CRC.
#define ALLOWLONG 1

// Set NOCRC to 1 to supporess sending 'C" instead of 'NAK', to indicate to sender to use checksum
// instead of CRC
#ifdef ALLOWLONG
#define NOCRC 0
#else
#define NOCRC 1
#endif

// Comment out to use XMODEM send
// #define YMODEM_SEND 1


#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18

// Packet lengths including SOH/STX, sequencen number and CRC
#define SHORT_PACKET_LEN 133
#define LONG_PACKET_LEN 1029

// Index into packet_buf for saving multiple packets for debugging

static int last_idx;

// Receive buffer

#ifdef ALLOWLONG
static unsigned char packet_buf[LONG_PACKET_LEN]; // Enough for STX 01 FE Data[1024] CRC CRC
#else
static unsigned char packet_buf[SHORT_PACKET_LEN]; // Enough for STX 01 FE Data[1024] CRC CRC
#endif

// Define size of debug code log
// Uncomment to disable debug log
#define DEBUG_LOG_SIZE 20

#ifdef DEBUG_LOG_SIZE

static void debug_logit(uint8_t state, uint8_t c);
static void debug_clrit();
static void debug_log_show();

#define DEBUG_LOGIT(state, c) debug_logit(state, c)
#define DEBUG_CLRIT() debug_clrit()
#define DEBUG_LOG_SHOW() debug_log_show()

#else

#define DEBUG_LOGIT(state, c)
#define DEBUG_CLRIT()
#define DEBUG_LOG_SHOW()

#endif

// Optional debug log

#ifdef DEBUG_LOG_SIZE

static struct debug_log_entry
{
    uint8_t state;
    uint8_t c;
} debug_log[DEBUG_LOG_SIZE];

static int debug_log_idx;

static void debug_logit(uint8_t state, uint8_t c)
{
    if (debug_log_idx != DEBUG_LOG_SIZE)
    {
        debug_log[debug_log_idx].state = state;
        debug_log[debug_log_idx].c = c;
    }
    ++debug_log_idx;
}

static void debug_clrit()
{
    debug_log_idx = 0;
}

static void debug_log_show()
{
    int x;
    if (last_idx) {
        nkinfile_t f[1];
        nkinfile_open_mem(f, packet_buf, last_idx);
        nk_byte_hex_dump(0, last_idx, f);
        
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
static char ymodem_nocrc = NOCRC;
static uint8_t ymodem_old_seq; // Seq. number of previous packet
static uint8_t ymodem_seq; // Next expected sequence number

// Ymodem send state

static uint8_t ymodem_send_eot;
static uint8_t ymodem_send_fin;
static uint8_t ymodem_send_seq;
static uint8_t ymodem_send_hdr;
static uint16_t ymodem_send_block_size;

static void ymodem_setup_header()
{
    packet_buf[0] = SOH;
    packet_buf[1] = ymodem_send_seq;
    packet_buf[2] = ~ymodem_send_seq;
#ifdef YMODEM_SEND
    unsigned short crc = nk_crc16be_check(packet_buf + 3, 128);
    packet_buf[131] = (crc >> 8);
    packet_buf[132] = crc;
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

#ifdef YMODEM_SEND
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
        packet_buf[3 + x] = c;
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

static int nk_ysend_evt(int (*tgetc)(void *), void *file, int c)
{
    int status = YMODEM_SEND_STATUS_MORE;
    DEBUG_LOGIT(
        (ymodem_send_eot << 0) |
        (ymodem_send_fin << 1),
        c);
    if (c == 'C' || c == NAK)
    {
        if (ymodem_send_eot)
        {
            nk_putc(EOT);
            DEBUG_LOGIT(4, 0x40);
        }
        else
        {
            // Retransmit
            ymodem_send_block();
            DEBUG_LOGIT(4, 0x41);
        }
    }
    else if (c == ACK)
    {
        if (ymodem_send_eot)
        {
            ymodem_send_eot = 0;
#ifdef YMODEM_SEND
            ymodem_prepare_fin();
            ymodem_send_fin = 1;
            ymodem_send_hdr = 1;
            // We don't send it here, other end if going to send C after the ACK
            DEBUG_LOGIT(4, 0x42);
#else
            status = YMODEM_SEND_STATUS_DONE;
            DEBUG_LOGIT(4, 0x43);
#endif
        }
        else if (ymodem_send_fin)
        {
            status = YMODEM_SEND_STATUS_DONE;
            DEBUG_LOGIT(4, 0x44);
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
                DEBUG_LOGIT(4, 0x45);
            }
            else
            {
                // Send EOT, we're done
                ymodem_send_eot = 1;
                // Zero length file right after ACK for header: wait for other end to request it with C
                if (ymodem_send_hdr)
                    ymodem_send_hdr = 0;
                else
                    nk_putc(EOT);
                DEBUG_LOGIT(4, 0x46);
            }
        }
    }
    else if (c == CAN)
    {
        // Cancel
        status = YMODEM_SEND_STATUS_CANCEL;
        DEBUG_LOGIT(4, 0x47);
    }
    else
    {
        // Junk
        // Just ignore it
        DEBUG_LOGIT(4, 0x48);
    }
    return status;
}

// Send a file using ymodem protocol

void nk_ysend_file(
    const char *name, 
    void *(*topen)(const char *name, const char *mode),
    void (*tclose)(void *f),
    int (*tgetc)(void *f),
    int (tsize)(void *f)
) {
    int sta;
    void *file = topen(name, "r");
    
    if (!file)
    {
        nk_printf("Couldn't open file\n");
        return;
    }

    // int async;
    // async = async_log_set(0);
    nk_set_uart_mode(1);

    nk_printf("Start YMODEM receive transfer...\r\n");
    nk_printf("Hit Ctrl-X to cancel.\r\n");

    DEBUG_CLRIT();
    ymodem_send_eot = 0;
    ymodem_send_fin = 0;

    // First block: file name
#ifdef YMODEM_SEND
    ymodem_send_seq = 0;
    memset(packet_buf + 3, 0, 128);
    nk_snprintf((char *)packet_buf + 3, 125, "%s%c%d", name, 0, tsize(file));
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

    do
    {
        int c = nk_getc();
        if (c != -1)
            sta = nk_ysend_evt(tgetc, file, c);
    } while (sta == YMODEM_SEND_STATUS_MORE);

    tclose(file);
    nk_set_uart_mode(0);
    // Re-enable background messages
    // async_log_set(async);

    // Delay to allow other end to restore terminal
    nk_udelay(1000000);
    nk_printf("\n");

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
}

// Funcitons for transferring a memory buffer as a file

static char *sdata;
static size_t sdata_offset;
static size_t sdata_size;
static int sdata_eof;

static void *tzopen(const char *name, const char *mode)
{
    sdata_offset = 0;
    sdata_eof = 0;
    return (void *)"fake";
}

static void tzclose(void *f)
{
}

static int tzgetc(void *file)
{
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
        nk_putc(NAK);
    else
        nk_putc('C');
    
}

// Checksum or CRC validation

static int crc_count; // Count of CRC packets
static int cksum_count; // Count of checksum packets

static int ymcrc_check(uint8_t *buf, int len)
{
    if (ymodem_nocrc)
    {
        int x;
        uint8_t cksum = 0;
        for (x = 0; x != len - 2; ++x)
            cksum += buf[x];
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

int ymodem_rcv(unsigned char *rcvbuf, int len)
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
                nk_putc(NAK); // Use C instead of NAK here to ensure CRC mode
            else
                nk_putc('C'); // Use C instead of NAK here to ensure CRC mode
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
        else if (len == 1 && rcvbuf[0] == CAN)
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
        else if ((len == SHORT_PACKET_LEN-ymodem_nocrc || len == LONG_PACKET_LEN-ymodem_nocrc) && rcvbuf[0] == SOH && rcvbuf[1] == 0 && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2])  && ymcrc_check(rcvbuf+3, SHORT_PACKET_LEN-3))
        {
            // Ymodem file header received
            ymodem_can = 0;
            ymodem_to = 0;
            // Valid open file packet
            if (rcvbuf[3] == 0)
            {
                // No more files
                nk_putc(ACK);
                status = YMODEM_RECV_DONE;
            }
            else if (ymodem_recv_file_open((char *)rcvbuf+3))
            {
                nk_putc(ACK);
                ymodem_old_seq = 0;
                ymodem_seq = 1;
                // We are going to send 'C' next: but do it after timeout
                ymodem_opened = 1;
                ymodem_first = 1;
            }
            else
            {
                nk_putc(CAN);
                status = YMODEM_RECV_OPEN_CANCEL;
            }
        }
        else if ((len == SHORT_PACKET_LEN-ymodem_nocrc || len == LONG_PACKET_LEN-ymodem_nocrc) && rcvbuf[0] == SOH && rcvbuf[1] == 1 && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2])  && ymcrc_check(rcvbuf+3, SHORT_PACKET_LEN-3))
        {
            // Xmodem first packet of anonymous file received
            ymodem_can = 0;
            ymodem_to = 0;
            // Valid open file packet
            if (rcvbuf[3] == 0)
            {
                // No more files
                nk_putc(ACK);
                status = YMODEM_RECV_DONE;
            }
            else if (ymodem_recv_file_open("anonymous\0"))
            {
                nk_putc(ACK);
                ymodem_old_seq = 1;
                ymodem_seq = 2;
                // We are going to send 'C' next: but do it after timeout
                ymodem_opened = 1;
                ymodem_xmodem = 1; // Xmodem mode
                ymodem_recv_file_write(rcvbuf + 3, len-(5 - ymodem_nocrc));
                ymodem_first = 0;
            }
            else
            {
                nk_putc(CAN);
                status = YMODEM_RECV_OPEN_CANCEL;
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
                    nk_putc(NAK);
                else
                    nk_putc('C');
            }
            else
            {
                nk_putc(NAK);
            }
        }
        else if (len == 1 && rcvbuf[0] == CAN)
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
        else if (len == SHORT_PACKET_LEN-ymodem_nocrc && rcvbuf[0] == SOH && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2]) && ymcrc_check(rcvbuf+3, SHORT_PACKET_LEN-3))
        {
            ymodem_got_eot = 0;
            ymodem_can = 0;
            if (rcvbuf[1] == ymodem_old_seq)
            {
                // Missed ACK- ignore data and resend ACK
                nk_putc(ACK);
            }
            else if (rcvbuf[1] != ymodem_seq)
            {
                // Out of sync
                nk_putc(CAN);
                ymodem_recv_file_cancel();
                status = YMODEM_RECV_SYNC_CANCEL;
            }
            else
            {
                // Good receive data
                ymodem_recv_file_write(rcvbuf + 3, 128);
                ymodem_first = 0;
                ymodem_old_seq = ymodem_seq++;
                nk_putc(ACK);
            }
        }
#ifdef ALLOWLONG
        else if (len == LONG_PACKET_LEN-ymodem_nocrc && rcvbuf[0] == STX && ((rcvbuf[1] ^ 0xFF) == rcvbuf[2]) && ymcrc_check(rcvbuf+3, LONG_PACKET_LEN-3))
        {
            ymodem_got_eot = 0;
            ymodem_can = 0;
            if (rcvbuf[1] == ymodem_old_seq)
            {
                // Missed ACK- ignore data and resend ACK
                nk_putc(ACK);
            }
            else if (rcvbuf[1] != ymodem_seq)
            {
                // Out of sync
                nk_putc(CAN);
                ymodem_recv_file_cancel();
                status = YMODEM_RECV_SYNC_CANCEL;
            }
            else
            {
                // Good receive data
                ymodem_recv_file_write(rcvbuf + 3, 1024);
                ymodem_first = 0;
                ymodem_old_seq = ymodem_seq++;
                nk_putc(ACK);
                ++long_count;
            }
        }
#endif
        else if (len == 1 && rcvbuf[0] == EOT)
        {
            ymodem_can = 0;
            if (ymodem_xmodem)
            {
                ymodem_recv_file_close();
                nk_putc(ACK);
                status = YMODEM_RECV_DONE;
            }
            else if (ymodem_got_eot)
            {
                ymodem_recv_file_close();
                nk_putc(ACK);
                ymodem_recv_init();
                ymodem_to = 0;
                
            }
            else
            {
                ymodem_got_eot = 1; // We want to see EOT twice
                nk_putc(NAK);
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

// Receive and process a file

int nk_yrecv()
{
    int sta = 0;
    // int async;

    // Disable background messages
    // async = async_log_set(0);

    // Disable NL->CR-LF mode
    nk_set_uart_mode(1);

    nk_printf("Start YMODEM send transfer...\r\n");
    nk_printf("Hit Ctrl-X twice to cancel.\r\n");

    last_idx = 0;
    long_count = 0;
    cksum_count = 0;
    crc_count = 0;
    ymodem_recv_init();

    do {
        int len;
        if (sta == YMODEM_RECV_PURGE) {
            // Purging- read as much as we can
            len = nk_uart_read((char *)packet_buf + last_idx, sizeof(packet_buf), NK_TIME_COUNTS_PER_SECOND*1000);
        } else {
            // Read one byte.  If we got SOH read 132 more.  If we got STX read 1028 more
            len = nk_uart_read((char *)packet_buf + last_idx, 1, NK_TIME_COUNTS_PER_SECOND*1000);
            if (len) {
                if (packet_buf[last_idx] == SOH) {
                    len = nk_uart_read((char *)packet_buf + last_idx + 1, SHORT_PACKET_LEN-1-ymodem_nocrc, NK_TIME_COUNTS_PER_SECOND*1000);
                    ++len;
#ifdef ALLOWLONG
                } else if (packet_buf[last_idx] == STX) {
                    len = nk_uart_read((char *)packet_buf + last_idx + 1, sizeof(packet_buf)-1-ymodem_nocrc, NK_TIME_COUNTS_PER_SECOND*1000);
                    ++len;
#endif
                } else if (packet_buf[last_idx] == CAN || packet_buf[last_idx] == EOT) {
                    // Return single character
                } else {
                    // Junk
                    len = nk_uart_read((char *)packet_buf + last_idx + 1, sizeof(packet_buf)-1, NK_TIME_COUNTS_PER_SECOND*1000);
                    ++len;
                }
            }
        }
        sta = ymodem_rcv(packet_buf + last_idx, len); // Call whenever we have some data...
#ifdef PROTOLOG
        last_idx += len;
#endif
    } while (sta == YMODEM_RECV_MORE || sta == YMODEM_RECV_PURGE);

    laststatus = sta;

    // Restore NL->CR-LF mode
    nk_set_uart_mode(0);

    // Time for other end to restore tty
    nk_udelay(1000000);

    // Re-enable background messages
    // async_log_set(async);
    return sta;
}  

void debug_rcv_status()
{
    nk_printf("status = %d\n", laststatus);
    nk_printf("long_count = %d\n", long_count);
    nk_printf("crc_count = %d\n", crc_count);
    nk_printf("cksum_count = %d\n", cksum_count);
    DEBUG_LOG_SHOW();
}
