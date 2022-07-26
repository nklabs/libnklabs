// Copyright 2021 NK Labs, LLC

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

// Ymodem Options:

// Uncomment to keep all ymodem receive data in packet_buf for debugging.  Received data is printed with "ymodem show"
// All received data must fit in the packet_buf.
// #define NK_YM_PROTOLOG 1

// Define size of debug event log
// Uncomment to disable event log
// For ymodem receive, this keeps a log of all sent characters
#define NK_YM_DEBUG_LOG_SIZE 200

// Allow 1K packets.  Comment out for only 128 byte packets
#define NK_YM_ALLOWLONG 1

// Define to supporess sending 'C" instead of 'NAK', to indicate to sender to use checksum instead of CRC
// #define NK_YM_NOCRC 1

// Comment out to use XMODEM send
#define NK_YM_YMODEM_SEND 1

// Ymodem startup retransmit timeout: default is supposed to be 10 seconds, but reducing makes startup faster
#define NK_YM_TIMEOUT 3
