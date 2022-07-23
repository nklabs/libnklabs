// Single address port 32-bit wide RAM with byte-enables

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

module ram_sp_be
  (
  clk,
  we,
  addr,
  wr_data,
  rd_data
  );

parameter ADDRWIDTH=8;
parameter WORDS = (1 << ADDRWIDTH);

input clk;
input [3:0] we;
input [ADDRWIDTH-1:0] addr;
input [31:0] wr_data;
output [31:0] rd_data;
reg [31:0] rd_data;

reg [31:0] mem[0:WORDS-1];

always @(posedge clk)
  begin
    rd_data <= mem[addr];
    if (we[0]) mem[addr][7:0] <= wr_data[7:0];
    if (we[1]) mem[addr][15:8] <= wr_data[15:8];
    if (we[2]) mem[addr][23:16] <= wr_data[23:16];
    if (we[3]) mem[addr][31:24] <= wr_data[31:24];
  end

endmodule
