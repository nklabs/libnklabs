// Declare the internal bus structure
// Break out the structure into wires

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

wire [BUS_IN_WIDTH-1:0] bus_in;

// No support for wor in Vivado!
//wor [BUS_OUT_WIDTH-1:0] bus_out; // wor used here so that we can have multiple drivers
//assign bus_out = 0; // In case nobody is driving.

wire [BUS_OUT_WIDTH-1:0] bus_out;

// Bus input fields

wire bus_reset_l = bus_in[BUS_FIELD_RESET_L]; // Reset
wire bus_clk = bus_in[BUS_FIELD_CLK]; // Clock

wire [BUS_DATA_WIDTH-1:0] bus_wr_data = bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START]; // Write data
wire [BUS_ADDR_WIDTH-1:0] bus_addr = bus_in[BUS_ADDR_END-1:BUS_ADDR_START]; // Address
wire bus_rd_req = bus_in[BUS_FIELD_RD_REQ]; // Read request
wire bus_wr_req = bus_in[BUS_FIELD_WR_REQ]; // Write request
wire [3:0] bus_be = bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE]; // Byte enables

// Bus output fields

wire bus_irq = bus_out[BUS_FIELD_IRQ]; // Interrupt request
wire bus_wr_ack = bus_out[BUS_FIELD_WR_ACK]; // Write acknowledge
wire bus_rd_ack = bus_out[BUS_FIELD_RD_ACK]; // Read acknowledge
wire [BUS_DATA_WIDTH-1:0] bus_rd_data = bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START]; // Read data
