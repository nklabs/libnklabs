// UART with FIFOs

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

module uart
  (
  clk,
  reset_l,

  ser_tx,
  ser_rx,

  tx_fifo_wr_data,
  tx_fifo_we,
  tx_fifo_af, // Tx FIFO is 1/2 full
  tx_fifo_cf, // Tx FIFO is completely full
  tx_fifo_ovf, // Pulse for every byte not written because FIFO is completely full
  tx_busy, // High if we are still transmitting

  rx_fifo_rd_data,
  rx_fifo_re,
  rx_fifo_ne, // Rx FIFO is not empty
  rx_fifo_af, // Rx FIFO is 1/2 full
  rx_fifo_ovf, // Pulse for every received byte lost because FIFO is completely full

  baud_rate,

  frame_error, // Receive frame error
  rx_en // High to enable receiver writing into FIFO
  );

parameter CPU_FREQ = 32'd25000000; // Frequency of 'clk' in Hz.

input clk;
input reset_l;

output ser_tx;
reg ser_tx;

input ser_rx;

input [7:0] tx_fifo_wr_data;
input tx_fifo_we;
output tx_fifo_af;
output tx_fifo_cf;
output tx_fifo_ovf;
output tx_busy;

output [7:0] rx_fifo_rd_data;
input rx_fifo_re;
output rx_fifo_ne;
output rx_fifo_af;
output rx_fifo_ovf;

input [31:0] baud_rate;

output frame_error;
reg frame_error;
input rx_en;

// Transmit FIFO

wire [7:0] tx_fifo_rd_data;
reg tx_fifo_re;
wire tx_fifo_ne;

fifo_sync_late #(.DATAWIDTH(8), .ADDRWIDTH(5), .SLOP(16)) tx_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

  .wr_data (tx_fifo_wr_data),
  .we (tx_fifo_we),
  .af (tx_fifo_af),
  .cf (tx_fifo_cf),
  .ovf (tx_fifo_ovf),

  .rd_data (tx_fifo_rd_data),
  .re (tx_fifo_re),
  .ne (tx_fifo_ne),
  .unf ()
  );

// Receive FIFO

reg [7:0] rx_fifo_wr_data;
reg rx_fifo_we;

fifo_sync_late #(.DATAWIDTH(8), .ADDRWIDTH(5), .SLOP(16)) rx_fifo
  (
  .clk (clk),
  .reset_l (reset_l),

  .wr_data (rx_fifo_wr_data),
  .we (rx_fifo_we),
  .af (rx_fifo_af),
  .ovf (rx_fifo_ovf),

  .rd_data (rx_fifo_rd_data),
  .re (rx_fifo_re),
  .ne (rx_fifo_ne),
  .unf ()
  );

// Transmitter

reg [8:0] tx_shift_reg;
reg [3:0] tx_counter;
reg [31:0] tx_baud_counter;

always @(posedge clk)
  if(!reset_l)
    begin
      ser_tx <= 1;
      tx_fifo_re <= 0;
      tx_counter <= 0;
      tx_shift_reg <= 0;
      tx_baud_counter <= 0;
    end
  else
    begin
      tx_fifo_re <= 0;
      if (tx_counter)
        begin
          // We're transmitting
          if (tx_baud_counter >= baud_rate)
            tx_baud_counter <= tx_baud_counter - baud_rate;
          else
            begin
              tx_baud_counter <= tx_baud_counter + CPU_FREQ - baud_rate;
              tx_counter <= tx_counter - 1;
              tx_shift_reg <= { 1'd1, tx_shift_reg[8:1] };
              ser_tx <= tx_shift_reg[0];
              // We have more data?
              if (tx_counter == 1 && tx_fifo_ne)
                begin
                  // Keep going..
                  tx_shift_reg <= { 1'd1, tx_fifo_rd_data };
                  ser_tx <= 0;
                  tx_fifo_re <= 1;
                  tx_counter <= 10;
                end
            end
        end
      else
        begin
          // We're not: try to read FIFO
          if (tx_fifo_ne)
            begin
              tx_shift_reg <= { 1'd1, tx_fifo_rd_data };
              ser_tx <= 0;
              tx_fifo_re <= 1;
              tx_counter <= 10;
              tx_baud_counter <= CPU_FREQ - baud_rate;
            end
        end
    end

wire tx_busy = (tx_fifo_ne || tx_counter != 0);

// Receiver

reg uart_rx_1;
reg uart_rx_2;

reg [8:0] rx_shift_reg;
reg [31:0] rx_baud_counter;
reg rx_ing; // High if we are receiving
reg rx_start; // High if we are receiving the start bit

always @(posedge clk or negedge reset_l)
  if(!reset_l)
    begin
      uart_rx_1 <= 1;
      uart_rx_2 <= 1;
      rx_shift_reg <= 9'h1FF;
      rx_baud_counter <= 0;
      rx_ing <= 0;
      rx_fifo_we <= 0;
      rx_fifo_wr_data <= 0;
      frame_error <= 0;
      rx_start <= 0;
    end
  else
    begin
      // Synchronize input
      uart_rx_2 <= ser_rx;
      uart_rx_1 <= uart_rx_2;
      rx_fifo_we <= 0;
      frame_error <= 0;

      if (rx_ing)
        begin
          if (rx_baud_counter >= baud_rate)
            rx_baud_counter <= rx_baud_counter - baud_rate;
          else if (!rx_shift_reg[0])
            // We've got start bit and stop bit
            begin
              rx_ing <= 0;
              rx_fifo_wr_data <= rx_shift_reg[8:1];
              rx_fifo_we <= rx_en;
              if(!uart_rx_1)
                frame_error <= rx_en;
            end
          else
            // Shift in next bit
            begin
              rx_baud_counter <= rx_baud_counter + CPU_FREQ - baud_rate;
              rx_shift_reg <= { uart_rx_1, rx_shift_reg[8:1] };
              // This is the middle of the start bit: fail if it's not still low
              if (rx_start)
                begin
                  rx_start <= 0;
                  if (uart_rx_1)
                    rx_ing <= 0;
                end
            end
        end
      else if (!uart_rx_1)
        begin
          // Leading edge of start bit
          // Delay 1/2 bit
          rx_baud_counter <= CPU_FREQ/32'd2 - baud_rate;
          rx_ing <= 1;
          rx_start <= 1;
          rx_shift_reg <= 9'h1ff;
        end
    end

endmodule
