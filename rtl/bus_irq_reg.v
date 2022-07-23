// Interrupt register

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

module bus_irq_reg
  (
  // Internal bus
  bus_in,
  bus_out,

  enable,	// Interrupt enable bits
  trig,		// Interrupt requests
  irq		// Interrupt request output
  );

parameter BUS_ADDR=0;
parameter OFFSET=0;
parameter DATAWIDTH=32;
parameter REG=0;
parameter SIZE=4;
parameter LEVEL = 0; // Bitmask indicating which interrupts are level sensitive

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input [DATAWIDTH-1:0] enable;

input [DATAWIDTH-1:0] trig;

output irq;
reg irq;

`include "bus_decl.v"

// Bus driver

reg [DATAWIDTH-1:0] cur;
reg [DATAWIDTH-1:0] out;

wire decode = ({ bus_addr[BUS_ADDR_WIDTH-1:2], 2'd0 } == BUS_ADDR);
wire rd_ack = (decode && bus_rd_req);
wire wr_ack = (decode && bus_wr_req);
reg reg_rd_ack;
reg reg_wr_ack;

assign bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START] = reg_rd_ack ? (out << OFFSET) : { BUS_DATA_WIDTH { 1'd0 } };
assign bus_out[BUS_FIELD_RD_ACK] = reg_rd_ack;
assign bus_out[BUS_FIELD_WR_ACK] = reg_wr_ack;
assign bus_out[BUS_FIELD_IRQ] = 0;

reg [3:0] count;

always @(posedge bus_clk)
  if(!bus_reset_l)
    begin
      cur <= 0;
      out <= 0;
      count <= 0;
      irq <= 0;
      reg_rd_ack <= 0;
      reg_wr_ack <= 0;
    end
  else
    begin
      reg_rd_ack <= rd_ack;
      reg_wr_ack <= wr_ack;
      if (count)
        count <= count - 1;

      irq <= (count == 0) && |(out & enable);

      out <= (cur & ~LEVEL) | (trig & LEVEL);

      if (cur & ~out)
        count <= 15; // New interrupt detected

      // Pulsed interrupts
      if(wr_ack)
        cur <= (cur ^ (bus_wr_data >> OFFSET)) | trig;
      else
        cur <= cur | trig;
    end

endmodule
