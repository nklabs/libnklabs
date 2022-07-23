// Instruction cache

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

module icache
  (
  reset_l,
  clk,

  // CPU side
  client_rd_req,
  client_addr, // Byte address
  client_rd_ack,
  client_rd_data,

  // Flash side
  spimem_addr, // Byte address
  spimem_rdata,
  spimem_ready,
  spimem_valid
  );

parameter LOG_SIZE_MEM = 24;
parameter SIZE_MEM = (1 << LOG_SIZE_MEM); // Size of memory in bytes
parameter LOG_LINESIZE = 5;
parameter LINESIZE = (1 << LOG_LINESIZE); // Cache line size in bytes
parameter LOG_LINES = 6;
parameter LINES = (1 << LOG_LINES); // Number of cache lines

parameter CACHE_SIZE = LINES * LINESIZE; // Cache size in bytes

parameter TAG_SIZE = (LOG_SIZE_MEM - (LOG_LINESIZE + LOG_LINES));

input reset_l;
input clk;

input client_rd_req;
input [LOG_SIZE_MEM-1:0] client_addr;
output client_rd_ack;
reg client_rd_ack;
output [31:0] client_rd_data;

output [LOG_SIZE_MEM-1:0] spimem_addr;
reg [LOG_SIZE_MEM-1:0] spimem_addr, n_spimem_addr;
input [31:0] spimem_rdata;
input spimem_ready;
output spimem_valid;
reg spimem_valid, n_spimem_valid;

// Tag RAM

wire [TAG_SIZE:0] tag_ram_rd_data;
reg [LOG_LINES-1:0] n_tag_ram_rd_addr, tag_ram_rd_addr;
reg [TAG_SIZE:0] n_tag_ram_wr_data, tag_ram_wr_data;
reg [LOG_LINES-1:0] n_tag_ram_wr_addr, tag_ram_wr_addr;
reg n_tag_ram_we;

ram_blk_dp #(.DATAWIDTH(1 + TAG_SIZE), .ADDRWIDTH(LOG_LINES)) tag_ram
  (
  .clk (clk),
  .wr_data (n_tag_ram_wr_data),
  .wr_addr (n_tag_ram_wr_addr),
  .we (n_tag_ram_we),
  .rd_data (tag_ram_rd_data),
  .rd_addr (n_tag_ram_rd_addr)
  );

// Cache RAM

reg [LOG_LINES + LOG_LINESIZE - 3:0] cache_ram_rd_addr, n_cache_ram_rd_addr;
reg [LOG_LINES + LOG_LINESIZE - 3:0] cache_ram_wr_addr, n_cache_ram_wr_addr;
reg n_cache_ram_we;

ram_blk_dp #(.DATAWIDTH(32), .ADDRWIDTH(LOG_LINES + LOG_LINESIZE - 2)) data_ram
  (
  .clk (clk),
  .wr_data (spimem_rdata),
  .wr_addr (cache_ram_wr_addr),
  .we (n_cache_ram_we),
  .rd_data (client_rd_data),
  .rd_addr (n_cache_ram_rd_addr)
  );

// Client side state machine

reg client_waiting, n_client_waiting;
reg [LOG_SIZE_MEM-1:0] client_hold_addr, n_client_hold_addr;

// Information about line currently being loaded
// _d to account for delay through memory
reg [LOG_LINESIZE-3:0] working_low, n_working_low, working_low_d;
reg [LOG_LINESIZE-3:0] working_high, n_working_high, working_high_d;
reg [LOG_SIZE_MEM-LOG_LINESIZE-1:0] working_line, n_working_line, working_line_d;

reg cache_ready, n_cache_ready; // Indicates that initial clearing is done

always @(posedge clk)
  if (!reset_l)
    begin
      tag_ram_rd_addr <= 0;
      cache_ram_rd_addr <= 0;
      client_hold_addr <= 0;
      client_waiting <= 0;
      working_low_d <= 0;
      working_high_d <= 0;
      working_line_d <= 0;
    end
  else
    begin
      tag_ram_rd_addr <= n_tag_ram_rd_addr;
      cache_ram_rd_addr <= n_cache_ram_rd_addr;
      client_hold_addr <= n_client_hold_addr;
      client_waiting <= n_client_waiting;
      working_low_d <= working_low;
      working_high_d <= working_high;
      working_line_d <= working_line;
    end

always @(*)
  begin
    n_tag_ram_rd_addr = tag_ram_rd_addr;
    n_cache_ram_rd_addr = cache_ram_rd_addr;
    n_client_waiting = client_waiting;
    n_client_hold_addr = client_hold_addr;
    client_rd_ack = 0;

    if (client_waiting)
      // We have read the RAMs, check that the line is valid and is for the correct address
      if (cache_ready && tag_ram_rd_data[TAG_SIZE] && tag_ram_rd_data[TAG_SIZE - 1:0] == client_hold_addr[LOG_SIZE_MEM - 1:LOG_LINES + LOG_LINESIZE])
        begin
          client_rd_ack = 1;
          n_client_waiting = 0;
        end
      // Handle partially full cache line- this happens when cache line is currently being loaded
      else if (cache_ready && working_line_d == client_hold_addr[LOG_SIZE_MEM - 1:LOG_LINESIZE] && (working_high_d - working_low_d) > (client_hold_addr[LOG_LINESIZE-1:2] - working_low_d))
        begin
          client_rd_ack = 1;
          n_client_waiting = 0;
        end

    // Register a new request- it can happen same cycle that previous request as acknowledged
    // Feed address right to cache ram and tag ram
    if (client_rd_req)
      begin
        n_tag_ram_rd_addr = client_addr[LOG_LINESIZE + LOG_LINES - 1:LOG_LINESIZE];
        n_cache_ram_rd_addr = client_addr[LOG_LINESIZE + LOG_LINES - 1:2];
        n_client_hold_addr = client_addr;
        n_client_waiting = 1;
      end
  end

// Flash side state machine

reg [2:0] state, n_state;

parameter
  RESET = 0,
  IDLE = 1,
  FILL = 2,
  PAUSE = 3;

always @(posedge clk)
  if (!reset_l)
    begin
      state <= RESET;
      tag_ram_wr_addr <= 0;
      tag_ram_wr_data <= 0;
      cache_ram_wr_addr <= 0;
      working_low <= 0;
      working_high <= 0;
      working_line <= 0;
      spimem_addr <= 0;
      spimem_valid <= 0;
      cache_ready <= 0;
    end
  else
    begin
      state <= n_state;
      tag_ram_wr_addr <= n_tag_ram_wr_addr;
      tag_ram_wr_data <= n_tag_ram_wr_data;
      cache_ram_wr_addr <= n_cache_ram_wr_addr;
      working_low <= n_working_low;
      working_high <= n_working_high;
      working_line <= n_working_line;
      spimem_addr <= n_spimem_addr;
      spimem_valid <= n_spimem_valid;
      cache_ready <= n_cache_ready;
    end

always @(*)
  begin
    n_state = state;
    n_tag_ram_wr_addr = tag_ram_wr_addr;
    n_tag_ram_wr_data = tag_ram_wr_data;
    n_tag_ram_we = 0;
    n_cache_ram_wr_addr = cache_ram_wr_addr;
    n_cache_ram_we = 0;
    n_working_low = working_low;
    n_working_high = working_high;
    n_working_line = working_line;
    n_spimem_addr = spimem_addr;
    n_spimem_valid = spimem_valid;
    n_cache_ready = cache_ready;

    case (state)
      RESET:
        begin
          n_tag_ram_we = 1;
          n_tag_ram_wr_addr = tag_ram_wr_addr + 1'd1;
          if (tag_ram_wr_addr + 1'd1 == 6'd0)
            begin
              n_state = IDLE;
              n_cache_ready = 1;
            end
        end

      IDLE:
        if (client_waiting && (!tag_ram_rd_data[TAG_SIZE] || tag_ram_rd_data[TAG_SIZE - 1:0] != client_hold_addr[LOG_SIZE_MEM - 1:LOG_LINES + LOG_LINESIZE]))
          begin
            // Line is invalid or has data for another address
            // So we need to fill this line...
            n_working_line = client_hold_addr[LOG_SIZE_MEM - 1:LOG_LINESIZE];
            n_working_low = client_hold_addr[LOG_LINESIZE-1:2];
            n_working_high = client_hold_addr[LOG_LINESIZE-1:2];
            n_spimem_addr = client_hold_addr[LOG_SIZE_MEM-1:0]; // Starting address to read
            n_spimem_valid = 1;
            n_tag_ram_wr_addr = client_hold_addr[LOG_LINESIZE + LOG_LINES - 1:LOG_LINESIZE];
            n_tag_ram_wr_data = { 1'b0, client_hold_addr[LOG_SIZE_MEM - 1:LOG_LINESIZE + LOG_LINES] };
            n_cache_ram_wr_addr = client_hold_addr[LOG_LINESIZE + LOG_LINES - 1:2];
            n_state = FILL;
          end

      FILL:
        if (spimem_ready)
          begin
            n_cache_ram_we = 1;
            n_tag_ram_we = 1; // Mark incompletely filled line as invalid
            n_cache_ram_wr_addr[LOG_LINESIZE - 3:0] = cache_ram_wr_addr[LOG_LINESIZE-3:0] + 1'd1;
            n_working_high = working_high + 1'd1;
            n_spimem_addr[LOG_LINESIZE-1:0] = spimem_addr[LOG_LINESIZE-1:0] + 3'd4;
            if (working_high + 1'd1 == working_low)
              begin
                n_state = PAUSE;
                n_spimem_valid = 0;
                n_tag_ram_wr_data[TAG_SIZE] = 1; // Line is now valid
                // Get spi flash to start prereading next line..
                n_spimem_addr[LOG_LINESIZE-1:0] = 0;
                n_spimem_addr[LOG_SIZE_MEM-1:LOG_LINESIZE] = spimem_addr[LOG_SIZE_MEM-1:LOG_LINESIZE] + 1'd1;
              end
          end

      PAUSE: // Allow time for valid to propagate through tag RAM otherwise we trigger a fill on the same row
        n_state = IDLE;
    endcase
  end

endmodule
