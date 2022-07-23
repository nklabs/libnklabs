// SPI slave interface

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

//   - Generates internal chip bus
// This version depends on having an internal clock faster than the SPI clock

module spi_slave
  (
  reset_l,
  clk,

  spi_reset_pin,
  spi_clk_pin,
  spi_din_pin,
  spi_dout_pin,

  bus_addr,
  bus_wr_data,
  bus_rd_data,
  bus_we,
  bus_re,
  bus_rd_ack
  );

parameter DATAWIDTH = 32;
parameter ADDRWIDTH = 16; // Min. is 3 bits, low two bits always 0

input reset_l;
input clk;

input spi_reset_pin;
reg spi_reset;
reg spi_reset_1;

input spi_clk_pin;
reg spi_clk;
reg spi_clk_1;
reg spi_clk_old;

input spi_din_pin;
reg spi_din;
reg spi_din_1;

output spi_dout_pin;
reg spi_dout;
wire spi_dout_pin = spi_dout;

output [ADDRWIDTH-1:0] bus_addr; // Least significant 2 bits are always 0!
reg [ADDRWIDTH-1:0] bus_addr;

output [DATAWIDTH-1:0] bus_wr_data;
reg [DATAWIDTH-1:0] bus_wr_data;

input [DATAWIDTH-1:0] bus_rd_data;

output bus_we;
reg bus_we;

output bus_re;
reg bus_re;

input bus_rd_ack;

reg [2:0] state;

parameter
  IDLE = 0,
  WRITE = 1,
  READ = 2,
  WRITE_REST = 3,
  READ_REST = 4
  ;

reg [DATAWIDTH-1:0] spi_shift_reg;
reg [DATAWIDTH-1:0] spi_out_reg;

always @(posedge clk)
  if (!reset_l)
    begin
      spi_reset <= 1;
      spi_reset_1 <= 1;

      spi_clk_1 <= 0;
      spi_clk <= 0;
      spi_clk_old <= 0;

      spi_din_1 <= 0;
      spi_din <= 0;

      spi_dout <= 0;

      bus_addr <= 0;
      bus_wr_data <= 0;
      bus_we <= 0;
      bus_re <= 0;

      state <= IDLE;

      spi_shift_reg <= 1;
      spi_out_reg <= 0;
    end
  else
    begin
      spi_reset_1 <= spi_reset_pin;
      spi_reset <= spi_reset_1;

      spi_clk_1 <= spi_clk_pin;
      spi_clk <= spi_clk_1;
      spi_clk_old <= spi_clk;

      spi_din_1 <= spi_din_pin;
      spi_din <= spi_din_1;

      bus_we <= 0;
      bus_re <= 0;

      // if (!spi_clk && spi_clk_old)
      //  spi_dout <= spi_out_reg[DATAWIDTH-1];

      if (spi_reset)
        begin
          spi_shift_reg <= 1;
          spi_out_reg <= 0;
          state <= IDLE;
        end
      else
        begin
          if (spi_clk && !spi_clk_old)
            begin
              spi_shift_reg <= { spi_shift_reg[DATAWIDTH-2:0], spi_din }; // Capture spi_din on rising clock edge
              spi_dout <= spi_out_reg[DATAWIDTH-1]; // Launch old spi_out_reg data on falling edge
              // Normally we would launch on falling edge, but because we are delayed due to input
              // synchronizer, after rising edge is OK.
              spi_out_reg <= { spi_out_reg[DATAWIDTH-2:0], 1'd0 }; // Shift output data
            end
          case (state)
            IDLE:
              if (spi_clk && !spi_clk_old)
                if (spi_shift_reg[ADDRWIDTH-2]) // Start bit...  addr bit 1 is on spi_din at this time
                  begin
                    bus_addr <= { spi_shift_reg[ADDRWIDTH-3:0], 2'd0 };
                    if (spi_din)
                      state <= WRITE;
                    else
                      begin
                        bus_re <= 1;
                        state <= READ;
                      end
                  end

            WRITE:
              if (spi_clk && !spi_clk_old)
                begin // Addr bit 0 is on spi_din at this time
                  spi_shift_reg <= 1;
                  state <= WRITE_REST;
                end

            READ:
              begin
                if (bus_rd_ack)
                  begin
                    spi_out_reg <= bus_rd_data;
                  end
                if (spi_clk && !spi_clk_old)
                  begin // Addr bit 0 is on spi_din at this time
                    spi_shift_reg <= 1;
                    state <= READ_REST;
                  end
              end

            WRITE_REST:
              if (spi_clk && !spi_clk_old)
                if (spi_shift_reg[DATAWIDTH-1])
                  begin // We have last bit
                    bus_we <= 1;
                    bus_wr_data <= { spi_shift_reg[DATAWIDTH-2:0], spi_din };
                    spi_shift_reg <= 1'd1;
                    state <= IDLE;
                  end

            READ_REST:
              if (spi_clk && !spi_clk_old)
                if (spi_shift_reg[DATAWIDTH-1])
                  begin // We've launched last bit
                    spi_shift_reg <= 1'd1;
                    state <= IDLE;
                  end
          endcase
        end
    end

endmodule
