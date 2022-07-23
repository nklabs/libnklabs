// These parameters define the internal bus

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

parameter BUS_DATA_WIDTH = 32;
parameter BUS_ADDR_WIDTH = 32;

// Bus input bits

parameter BUS_WR_DATA_START = 0; // Write data
parameter BUS_WR_DATA_END = BUS_WR_DATA_START + BUS_DATA_WIDTH;

parameter BUS_ADDR_START = BUS_WR_DATA_END; // Address
parameter BUS_ADDR_END = BUS_ADDR_START + BUS_ADDR_WIDTH;

parameter BUS_IN_CTRL_START = BUS_ADDR_END; // Control bits
parameter BUS_IN_CTRL_END = BUS_IN_CTRL_START + 8;

parameter BUS_FIELD_RESET_L = BUS_IN_CTRL_START + 0; // Synchronous
parameter BUS_FIELD_CLK = BUS_IN_CTRL_START + 1; // Clock
parameter BUS_FIELD_RD_REQ = BUS_IN_CTRL_START + 2; // Read enable
parameter BUS_FIELD_WR_REQ = BUS_IN_CTRL_START + 3; // Write enable
parameter BUS_FIELD_BE = BUS_IN_CTRL_START + 4; // Byte enables

parameter BUS_IN_WIDTH = BUS_IN_CTRL_END; // Size of bus_in

// Bus output bits

parameter BUS_RD_DATA_START = 0; // Read data
parameter BUS_RD_DATA_END = BUS_RD_DATA_START + BUS_DATA_WIDTH;
parameter BUS_OUT_CTRL_START = BUS_RD_DATA_END; // Control bits
parameter BUS_OUT_CTRL_END = BUS_OUT_CTRL_START + 3;

parameter BUS_OUT_WIDTH = BUS_OUT_CTRL_END; // Size of bus_out

parameter BUS_FIELD_WR_ACK = BUS_OUT_CTRL_START + 0; // Write ack
parameter BUS_FIELD_RD_ACK = BUS_OUT_CTRL_START + 1; // Read ack
parameter BUS_FIELD_IRQ = BUS_OUT_CTRL_START + 2; // IRQ
