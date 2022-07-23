// Addressing window which crosses clock domains

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

module bus_window_async
  (
  sub_clk,		// Clock for sub bus
  sub_reset_l,		// Reset for sub bus
  
  bus_in,
  bus_out,

  sub_bus_in,
  sub_bus_out
  );

`include "bus_params.v"

parameter BUS_ADDR = 0;		// Base address
parameter ADDRWIDTH = 8;	// Size (1<<ADDRWIDTH) bytes

input sub_clk;
input sub_reset_l;

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output [BUS_IN_WIDTH-1:0] sub_bus_in;
input [BUS_OUT_WIDTH-1:0] sub_bus_out;

`include "bus_decl.v"

// Return

wire sub_irq;
wire sub_wr_ack;
wire sub_rd_ack;
reg [BUS_DATA_WIDTH-1:0] sub_rd_data;

assign bus_out[BUS_FIELD_IRQ] = sub_irq;
assign bus_out[BUS_FIELD_WR_ACK] = sub_wr_ack;
assign bus_out[BUS_FIELD_RD_ACK] = sub_rd_ack;
assign bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START] = sub_rd_ack ? sub_rd_data : { BUS_DATA_WIDTH { 1'd0 } };

// Forward

// Decode if upper bits match
wire decode = (bus_addr & ~((1 << ADDRWIDTH) - 1)) == (BUS_ADDR & ~((1 << ADDRWIDTH) - 1));

wire sub_rd_req;
wire sub_wr_req;
reg [BUS_DATA_WIDTH-1:0] sub_wr_data;
reg [BUS_ADDR_WIDTH-1:0] sub_addr;
reg [3:0] sub_be;

assign sub_bus_in[BUS_FIELD_RESET_L] = sub_reset_l;
assign sub_bus_in[BUS_FIELD_CLK] = sub_clk;
assign sub_bus_in[BUS_FIELD_RD_REQ] = sub_rd_req;
assign sub_bus_in[BUS_FIELD_WR_REQ] = sub_wr_req;
assign sub_bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START] = sub_wr_data;
assign sub_bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = sub_addr;
assign sub_bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE] = sub_be;

// Main side of bus

wire rd_req = bus_in[BUS_FIELD_RD_REQ];
wire wr_req = bus_in[BUS_FIELD_WR_REQ];

// Drive slave bus

always @(posedge bus_clk)
  if (!bus_reset_l)
    begin
      sub_wr_data <= 0;
      sub_addr <= 0;
      sub_be <= 0;
    end
  else if (rd_req || wr_req)
    begin
      sub_addr <= (BUS_ADDR & ~((32'd1 << ADDRWIDTH) - 1)) | (bus_addr & ((1 << ADDRWIDTH) - 1));
      sub_be <= bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE];
      sub_wr_data <= bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START];
    end

// Master bus to slave bus

pulse_sync rd_pulse_sync
  (
  .i_clk (bus_clk),
  .i_reset_l (bus_reset_l),
  .i (rd_req && decode),

  .o_clk (sub_clk),
  .o_reset_l (sub_reset_l),
  .o (sub_rd_req)
  );

pulse_sync wr_pulse_sync
  (
  .i_clk (bus_clk),
  .i_reset_l (bus_reset_l),
  .i (wr_req && decode),

  .o_clk (sub_clk),
  .o_reset_l (sub_reset_l),
  .o (sub_wr_req)
  );

// slave bus back to master bus

pulse_sync wr_ack_pulse_sync
  (
  .i_clk (sub_clk),
  .i_reset_l (sub_reset_l),
  .i (sub_bus_out[BUS_FIELD_WR_ACK]),

  .o_clk (bus_clk),
  .o_reset_l (bus_reset_l),
  .o (sub_wr_ack)
  );

pulse_sync rd_ack_pulse_sync
  (
  .i_clk (sub_clk),
  .i_reset_l (sub_reset_l),
  .i (sub_bus_out[BUS_FIELD_RD_ACK]),

  .o_clk (bus_clk),
  .o_reset_l (bus_reset_l),
  .o (sub_rd_ack)
  );

simple_sync sub_irq_syncer
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),
  .i (sub_bus_out[BUS_FIELD_IRQ]),
  .o (sub_irq)
  );

// Register return data

reg [31:0] sub_rd_data_hold;

always @(posedge sub_clk)
  if(!sub_reset_l)
    sub_rd_data_hold <= 0;
  else if(sub_bus_out[BUS_FIELD_RD_ACK])
    sub_rd_data_hold <= sub_bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START];

always @(posedge bus_clk)
  if (!bus_reset_l)
    sub_rd_data <= 0;
  else
    sub_rd_data <= sub_rd_data_hold;

endmodule
