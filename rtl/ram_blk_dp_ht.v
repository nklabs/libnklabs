// Parameterized dual-port RAM

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

module ram_blk_dp_ht #(parameter DATAWIDTH = 8, ADDRWIDTH = 9)
  (
  input clk,
  input enable,
  input [DATAWIDTH-1:0] wr_data,
  input [ADDRWIDTH-1:0] wr_addr,
  input we,
  output reg [DATAWIDTH-1:0] rd_data,
  input [ADDRWIDTH-1:0] rd_addr
  );

reg [DATAWIDTH-1:0] ram[((1 << ADDRWIDTH) - 1) : 0];

wire [DATAWIDTH-1:0] wr_data_d = wr_data & { DATAWIDTH { enable } };
wire [ADDRWIDTH-1:0] wr_addr_d = wr_addr & { ADDRWIDTH { enable } };
wire [ADDRWIDTH-1:0] rd_addr_d = rd_addr & { ADDRWIDTH { enable } };
wire we_d = enable & we;

always @(posedge clk)
  begin
    if (we_d)
      ram[wr_addr_d] <= wr_data_d;
    rd_data <= ram[rd_addr_d];
  end

endmodule
