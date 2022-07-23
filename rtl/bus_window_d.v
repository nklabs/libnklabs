// Addressing window with one pipeline delay

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

// Create a new sub-bus which is focused on a particular address range.
// Address bits not needed for the range are set to constants which reduces
// the size of address decoders on the sub-bus.

// Registers on the sub-bus should use the full register address, not an
// offset relative to the window's address.

module bus_window_d
  (
  bus_in,
  bus_out,

  sub_bus_in,
  sub_bus_out
  );

`include "bus_params.v"

parameter BUS_ADDR = 0;		// Base address, must be a multiple of (1 << ADDRWIDTH)
parameter ADDRWIDTH = 8;	// Size (1<<ADDRWIDTH) bytes

input [BUS_IN_WIDTH-1:0] bus_in;

output [BUS_OUT_WIDTH-1:0] bus_out;

output [BUS_IN_WIDTH-1:0] sub_bus_in;

input [BUS_OUT_WIDTH-1:0] sub_bus_out;

`include "bus_decl.v"

// Outputs

reg [BUS_OUT_WIDTH-1:0] sub_bus_out_d;

assign bus_out = sub_bus_out_d;

always @(posedge bus_clk)
  if(!bus_reset_l)
    sub_bus_out_d <= 0;
  else
    sub_bus_out_d <= sub_bus_out;

// Inputs

assign sub_bus_in[BUS_FIELD_RESET_L] = bus_reset_l;
assign sub_bus_in[BUS_FIELD_CLK] = bus_clk;

// Decode if upper bits match
wire decode = (bus_addr & ~((1 << ADDRWIDTH) - 1)) == (BUS_ADDR & ~((1 << ADDRWIDTH) - 1));

reg bus_rd_req_d;
reg bus_wr_req_d;
reg [3:0] bus_be_d;
reg [BUS_DATA_WIDTH-1:0] bus_wr_data_d;
reg [BUS_ADDR_WIDTH-1:0] bus_addr_d;

always @(posedge bus_clk)
  if(!bus_reset_l)
    begin
      bus_rd_req_d <= 0;
      bus_wr_req_d <= 0;
      bus_wr_data_d <= 0;
      bus_addr_d <= 0;
      bus_be_d <= 0;
    end
  else
    begin
      bus_rd_req_d <= bus_rd_req && decode;
      bus_wr_req_d <= bus_wr_req && decode;
      bus_wr_data_d <= bus_wr_data;
      bus_addr_d <= (BUS_ADDR & ~((1 << ADDRWIDTH) - 1)) | (bus_addr & ((1 << ADDRWIDTH) - 1));
      bus_be_d <= bus_be;
    end

assign sub_bus_in[BUS_FIELD_RD_REQ] = bus_rd_req_d;
assign sub_bus_in[BUS_FIELD_WR_REQ] = bus_wr_req_d;
assign sub_bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START] = bus_wr_data_d;
assign sub_bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = bus_addr_d;
assign sub_bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE] = bus_be_d;

endmodule
