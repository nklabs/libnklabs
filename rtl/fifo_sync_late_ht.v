// A parameterized FIFO

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

// One flop delay between re and rd_data/ne.
// Two flop delay between we and rd_data/ne.

module fifo_sync_late_ht
  #(parameter ADDRWIDTH = 5, // Address width: FIFO will have 2^ADDRWIDTH words
              DATAWIDTH = 18, // Data width
              SLOP = 4) // No. words between full and overflow
  (
  input clk,
  input reset_l,
  input enable,

  input [DATAWIDTH-1:0] wr_data, // Write data
  input we, // Write enable
  output reg af, // Almost full
  output reg cf, // Completely full 
  output reg ovf, // Pulses for each word not written due to FIFO being completely full

  output [DATAWIDTH-1:0] rd_data, // Read data registered
  input re, // Read enable
  output reg ne, // Not empty
  output reg unf // Pulses for each attempt to read from empty FIFO
  );

reg [ADDRWIDTH:0] rd_addr, ns_rd_addr; // Points to oldest data
reg [ADDRWIDTH:0] wr_addr, ns_wr_addr;
reg [ADDRWIDTH:0] wrside_count, ns_wrside_count;
reg [ADDRWIDTH:0] rdside_count, ns_rdside_count;

ram_blk_dp_ht #(.DATAWIDTH(DATAWIDTH), .ADDRWIDTH(ADDRWIDTH)) ram
  (
  .clk (clk),
  .enable (enable),
  .wr_data (wr_data),
  .wr_addr (wr_addr[ADDRWIDTH-1:0]),
  .we (we && !cf),
  .rd_data (rd_data),
  .rd_addr (ns_rd_addr[ADDRWIDTH-1:0])
  );

// State machine

always @(posedge clk)
  if (!reset_l)
    begin
      rd_addr <= 0;
      wrside_count <= 0;
      rdside_count <= 0;
      wr_addr <= 0;
    end
  else
    begin
      wr_addr <= ns_wr_addr;
      rd_addr <= ns_rd_addr;
      wrside_count <= ns_wrside_count;
      rdside_count <= ns_rdside_count;
    end

// Read side state

always @(*)
  begin
    ns_rd_addr = rd_addr;
    ns_wr_addr = wr_addr;
    ns_wrside_count = wrside_count;
    ns_rdside_count = rdside_count;
    unf = 0;
    ovf = 0;

    ne = (rdside_count != 0);
    cf = wrside_count[ADDRWIDTH];

    if (re)
      if (ne)
        ns_rd_addr = ns_rd_addr + 1;
      else
        unf = 1;

    if (we)
      if (cf)
        ovf = 1;
      else
        ns_wr_addr = ns_wr_addr + 1;

    ns_rdside_count = wr_addr - ns_rd_addr; // 1-flop delay from re
    ns_wrside_count = ns_wr_addr - rd_addr; // 1-flop delay from we

    af = (wrside_count >= (1 << ADDRWIDTH) - SLOP);
  end

endmodule
