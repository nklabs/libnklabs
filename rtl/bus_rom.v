// Bus accessible ROM

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

module bus_rom
  (
  bus_in,
  bus_out
  );

parameter INIT_FILE = "notfound.mem";
parameter BUS_ADDR = 0;
parameter LOGSIZE = 16; // Log2 of memory size in bytes
parameter SIZE = (1 << LOGSIZE); // Size of this memory in bytes

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

`include "bus_decl.v"

wire decode = (bus_addr >= BUS_ADDR && bus_addr < (BUS_ADDR + SIZE));
wire rd_ack = (decode && bus_rd_req);

reg reg_rd_ack;

always @(posedge bus_clk)
  if (!bus_reset_l)
    begin
      reg_rd_ack <= 0;
    end
  else
    begin
      reg_rd_ack <= rd_ack;
    end

wire [31:0] ram_rd_data;

assign bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START] = reg_rd_ack ? ram_rd_data : 32'd0;
assign bus_out[BUS_FIELD_RD_ACK] = reg_rd_ack;
assign bus_out[BUS_FIELD_WR_ACK] = 0;
assign bus_out[BUS_FIELD_IRQ] = 0;

rom #(.ADDRWIDTH(LOGSIZE-2), .INIT_FILE(INIT_FILE)) rom
  (
  .clk (bus_clk),
  .rd_addr (bus_addr[LOGSIZE-1:2]),
  .rd_data (ram_rd_data)
  );

endmodule
