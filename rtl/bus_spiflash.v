// Bus accessible SPI Flash

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

module bus_spiflash
  (
  bus_in,
  bus_out,

  flash_cs_l,
  flash_clk,

  flash_io0_oe,
  flash_io1_oe,
  flash_io2_oe,
  flash_io3_oe,

  flash_io0_do,
  flash_io1_do,
  flash_io2_do,
  flash_io3_do,

  flash_io0_di,
  flash_io1_di,
  flash_io2_di,
  flash_io3_di
  );

parameter BUS_ADDR_CFG = 0; // Config register I/O address
parameter SIZE_CFG = 4;

parameter BUS_ADDR_MEM = 32'h0010_0000; // Where memory is located on the bus
parameter SIZE_MEM = 32'h0010_0000; // How much bus space allocated for the memory
parameter BANK0_OFFSET = 32'h0060_0000; // Offset within flash to our memory
parameter BANK1_OFFSET = 32'h00E0_0000; // Offset within flash to our memory

`include "bus_params.v"

input [BUS_IN_WIDTH-1:0] bus_in;
output [BUS_OUT_WIDTH-1:0] bus_out;

output flash_cs_l;
output flash_clk;

output flash_io0_oe;
output flash_io1_oe;
output flash_io2_oe;
output flash_io3_oe;

output flash_io0_do;
output flash_io1_do;
output flash_io2_do;
output flash_io3_do;

input  flash_io0_di;
input  flash_io1_di;
input  flash_io2_di;
input  flash_io3_di;

`include "bus_decl.v"

wire spimem_ready; // High when flash data is available (high only when spimem_valid is high)
reg bank_read; // High if we are waiting to read the active bank
reg mem_read; // High if no longer waiting for active bank, so normal memory access is available
wire mem_ready = (spimem_ready && mem_read); // Gated spimem_ready for normal memory access
reg bank; // The bank that the firmware is located in

// Flash config register

wire decode_cfg = ({ bus_addr[BUS_ADDR_WIDTH-1:2], 2'd0 } == BUS_ADDR_CFG);
wire decode_mem = (bus_addr >= BUS_ADDR_MEM && bus_addr < (BUS_ADDR_MEM + SIZE_MEM));

wire mem_rd_req = decode_mem && bus_rd_req;

wire cfg_wr_ack = decode_cfg && bus_wr_req;
wire cfg_rd_ack = decode_cfg && bus_rd_req;
reg cfg_rd_ack_reg;
reg cfg_wr_ack_reg;

wire [3:0] cfgreg_we = (bus_be & { 4 { cfg_wr_ack } });
wire [31:0] cfgreg_do;

always @(posedge bus_clk)
  if (!bus_reset_l)
    begin
      cfg_rd_ack_reg <= 0;
      cfg_wr_ack_reg <= 0;
    end
  else
    begin
      cfg_rd_ack_reg <= cfg_rd_ack;
      cfg_wr_ack_reg <= cfg_wr_ack;
    end

wire [31:0] spimem_rdata;

`ifdef NOCACHE

reg [23:0] spimem_addr;
reg spimem_valid;

always @(posedge bus_clk)
  if (!bus_reset_l)
    begin
      spimem_valid <= 0;
      spimem_addr <= 0;
    end
  else
    begin
      spimem_valid <= mem_rd_req || (spimem_valid && !mem_ready);
      if (mem_rd_req)
        spimem_addr <= bus_addr - BUS_ADDR_MEM + BANK0_OFFSET;
    end

assign bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START] = mem_ready ? spimem_rdata : (cfg_rd_ack_reg ? cfgreg_do : 32'd0);
assign bus_out[BUS_FIELD_RD_ACK] = cfg_rd_ack_reg | mem_ready;

`else

// A cache in front of spimemio

wire [23:0] spimem_addr;
wire spimem_valid;

wire mem_rd_ack;
wire [31:0] mem_rd_data;
wire [23:0] client_addr = bus_addr - BUS_ADDR_MEM + BANK0_OFFSET;

icache icache
  (
  .reset_l (bus_reset_l),
  .clk (bus_clk),

  // CPU side
  .client_rd_req (mem_rd_req),
  .client_addr (client_addr),
  .client_rd_ack (mem_rd_ack),
  .client_rd_data (mem_rd_data),

  // Flash side
  .spimem_addr (spimem_addr), // Byte address
  .spimem_rdata (spimem_rdata),
  .spimem_ready (mem_ready),
  .spimem_valid (spimem_valid)
  );

assign bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START] = mem_rd_ack ? mem_rd_data : (cfg_rd_ack_reg ? cfgreg_do : 32'd0);
assign bus_out[BUS_FIELD_RD_ACK] = cfg_rd_ack_reg | mem_rd_ack;

`endif

assign bus_out[BUS_FIELD_WR_ACK] = cfg_wr_ack_reg;
assign bus_out[BUS_FIELD_IRQ] = 0;

spimemio spimemio
  (
  .clk (bus_clk),
  .resetn (bus_reset_l),

  .valid (bank_read || spimem_valid),
  .ready (spimem_ready),
  .addr (bank_read ? 24'hFE0000 : ({ spimem_addr[23] ^ bank, spimem_addr[22:0] })),
  .rdata (spimem_rdata),

  .flash_csb (flash_cs_l),
  .flash_clk (flash_clk),

  .flash_io0_oe (flash_io0_oe),
  .flash_io1_oe (flash_io1_oe),
  .flash_io2_oe (flash_io2_oe),
  .flash_io3_oe (flash_io3_oe),

  .flash_io0_do (flash_io0_do),
  .flash_io1_do (flash_io1_do),
  .flash_io2_do (flash_io2_do),
  .flash_io3_do (flash_io3_do),

  .flash_io0_di (flash_io0_di),
  .flash_io1_di (flash_io1_di),
  .flash_io2_di (flash_io2_di),
  .flash_io3_di (flash_io3_di),

  .cfgreg_we (cfgreg_we),
  .cfgreg_di (bus_wr_data),
  .cfgreg_do (cfgreg_do),
  .bank (bank)
  );

// Read active bank

always @(posedge bus_clk)
  if (!bus_reset_l)
    begin
      bank <= 0;
      bank_read <= 1;
      mem_read <= 0;
    end
  else
    begin
      mem_read <= !bank_read;
      if (bank_read && spimem_ready)
        begin
          bank_read <= 0;
          bank <= spimem_rdata[0];
        end
    end

endmodule
