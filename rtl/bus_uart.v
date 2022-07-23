// Complete UART

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

module bus_uart
  (
  bus_in,
  bus_out,

  // Serial

  ser_rx,
  ser_tx,

  // Pending receive characters available

  recv_buf_valid
  );

parameter BUS_ADDR = 0;
parameter SIZE = 8;
parameter CPU_FREQ = 25000000; // Used for baud generation

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

input ser_rx;
output ser_tx;

output recv_buf_valid;

`include "bus_decl.v"

// The actual UART

wire [31:0] baud_rate;
wire frame_error;
wire rx_en;

wire tx_fifo_af;
wire tx_fifo_cf;
wire tx_fifo_ovf;
wire tx_fifo_we;
wire [31:0] tx_fifo_wr_data;
wire tx_busy;

wire rx_fifo_af;
wire rx_fifo_ovf;
wire rx_fifo_re;
wire [7:0] rx_fifo_rd_data_1;
wire rx_fifo_ne;

uart raw_uart
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),

  // Baud rate
  .baud_rate (baud_rate),

  .ser_tx (ser_tx),
  .ser_rx (ser_rx),

  // Tx side
  .tx_fifo_wr_data (tx_fifo_wr_data[7:0]),
  .tx_fifo_we (tx_fifo_we),
  .tx_fifo_af (tx_fifo_af),
  .tx_fifo_cf (tx_fifo_cf),
  .tx_fifo_ovf (tx_fifo_ovf),
  .tx_busy (tx_busy),

  // Rx side
  .rx_fifo_rd_data (rx_fifo_rd_data_1),
  .rx_fifo_re (rx_fifo_re && rx_fifo_ne),
  .rx_fifo_ne (rx_fifo_ne), // Rx FIFO is not empty
  .rx_fifo_af (rx_fifo_af), // Rx FIFO is 1/2 full
  .rx_fifo_ovf (rx_fifo_ovf), // Pulse for every received byte lost because FIFO is completely full

  .frame_error (frame_error),
  .rx_en (rx_en)
  );

wire recv_buf_valid = rx_fifo_ne;

// Programmable registers

// Control register

wire [BUS_OUT_WIDTH-1:0] uart_ctrl_out;

wire [31:0] ctrl_reg_out;
wire [31:0] ctrl_reg_in;

assign ctrl_reg_in[27:0] = ctrl_reg_out[27:0]; // Baud rate
assign baud_rate = { 4'd0, ctrl_reg_out[27:0] };
assign rx_en = ctrl_reg_out[28];

assign ctrl_reg_in[28] = rx_en; // High to enable receiver (including receive interrupts)
assign ctrl_reg_in[29] = tx_fifo_cf; // High means absolutely no more TX FIFO space
assign ctrl_reg_in[30] = tx_fifo_af; // Low means we have space for at least 16 more characters
assign ctrl_reg_in[31] = tx_busy; // Low means FIFO is empty and transmitter is idle (safe to change baud rate)

bus_split_reg #(.BUS_ADDR(BUS_ADDR), .IZ(32'h1_001c200)) uart_ctrl_reg
  (
  .bus_in (bus_in),
  .bus_out (uart_ctrl_out),

  .in (ctrl_reg_in),
  .out (ctrl_reg_out)
  );

// Read -1 if there is no read data

wire [31:0] rx_fifo_rd_data = rx_fifo_ne ? { 24'd0, rx_fifo_rd_data_1 } : 32'hffffffff;

// Data register

wire [BUS_OUT_WIDTH-1:0] uart_data_out;

bus_split_reg #(.BUS_ADDR(BUS_ADDR+4)) uart_data_reg
  (
  .bus_in (bus_in),
  .bus_out (uart_data_out),

  .in (rx_fifo_rd_data),
  .out (tx_fifo_wr_data),

  .rd_pulse (rx_fifo_re),
  .wr_pulse (tx_fifo_we)
  );

assign bus_out = uart_data_out | uart_ctrl_out;

endmodule
