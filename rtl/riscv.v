// RISCV CPU Complex

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

module riscv
  (
  cpu_clk,
  bus_clk,
  reset_l,

  uart_rx,
  uart_tx,

  flash_clk,
  flash_cs_l,

  flash_di0,
  flash_di1,
  flash_di2,
  flash_di3,

  flash_do0,
  flash_do1,
  flash_do2,
  flash_do3,

  flash_oe0,
  flash_oe1,
  flash_oe2,
  flash_oe3,

  led_reg_out,

  bus_in,
  bus_out,

  net_irq
  );

parameter CPU_FREQ = 25000000;

`include "bus_params.v"

input cpu_clk;
input bus_clk;
input reset_l;

input uart_rx;
output uart_tx;

output flash_clk;
output flash_cs_l;

input flash_di0;
input flash_di1;
input flash_di2;
input flash_di3;

output flash_do0;
output flash_do1;
output flash_do2;
output flash_do3;

output flash_oe0;
output flash_oe1;
output flash_oe2;
output flash_oe3;

output [31:0] led_reg_out;

output [BUS_IN_WIDTH-1:0] bus_in;
input [BUS_OUT_WIDTH-1:0] bus_out;

// Peripheral bus
wire [BUS_IN_WIDTH-1:0] bus_in; // Combined bus
wire [BUS_OUT_WIDTH-1:0] bus_out;

input net_irq;

// Local bus
wire [BUS_IN_WIDTH-1:0] cpu_bus_in; // Combined bus
wire [BUS_OUT_WIDTH-1:0] cpu_bus_out;

assign cpu_bus_in[BUS_FIELD_RESET_L] = reset_l;
assign cpu_bus_in[BUS_FIELD_CLK] = cpu_clk;

// CPU signals

wire [31:0] mem_wdata;
wire [3:0] mem_be;
wire [31:0] mem_addr;
wire uart_irq;
wire [31:0] irq = { 27'd0, net_irq, uart_irq, 3'd0 };

wire [31:0] mem_rdata;
wire mem_read;
wire mem_write;
wire mem_ready;

assign cpu_bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE] = mem_be;
assign cpu_bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = mem_addr;
assign cpu_bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START] = mem_wdata;
assign cpu_bus_in[BUS_FIELD_RD_REQ] = mem_read;
assign cpu_bus_in[BUS_FIELD_WR_REQ] = mem_write;

assign mem_rdata = cpu_bus_out[BUS_RD_DATA_END-1:BUS_RD_DATA_START];
assign mem_ready = cpu_bus_out[BUS_FIELD_WR_ACK] | cpu_bus_out[BUS_FIELD_RD_ACK];

// RISC-V CPU

picorv32 #(
  .STACKADDR (32'h0001_0000), // End of RAM, initial SP value
  .PROGADDR_RESET (32'h0010_0000), // Start of ROM, initial PC value
  .PROGADDR_IRQ (32'h0010_0010),
  .BARREL_SHIFTER (1),
  .COMPRESSED_ISA (1),
  .ENABLE_COUNTERS (1),
  .ENABLE_MUL (1),
  .ENABLE_DIV (1),
  .ENABLE_IRQ (1),
  .ENABLE_IRQ_QREGS (0),
  .LATCHED_IRQ (32'hffff_ffe7) // NET IRQ and UART IRQ are level sensitive
) cpu
  (
  .clk (cpu_clk),
  .resetn (reset_l),

  .trap (),
  
  .mem_ready (mem_ready),
  .mem_rdata (mem_rdata),

  .mem_valid (),
  .mem_instr (),
  .mem_addr (),
  .mem_wdata (),
  .mem_wstrb (),

  .mem_la_read (mem_read),
  .mem_la_write (mem_write),
  .mem_la_addr (mem_addr),
  .mem_la_wdata (mem_wdata),
  .mem_la_wstrb (mem_be),

  .irq (irq),
  .eoi (),

  .pcpi_valid (),
  .pcpi_insn (),
  .pcpi_rs1 (),
  .pcpi_rs2 (),
  .pcpi_wr (),
  .pcpi_rd (),
  .pcpi_wait (),
  .pcpi_ready (),

  .trace_valid (),
  .trace_data ()
  );

// Software's RAM

wire [BUS_OUT_WIDTH-1:0] cpu_ram_bus_out;

bus_ram #(.BUS_ADDR(32'h0000_0000), .LOGSIZE(16)) cpu_ram
  (
  .bus_in (cpu_bus_in),
  .bus_out (cpu_ram_bus_out)
  );

// Software access to SPI Flash

wire [BUS_OUT_WIDTH-1:0] cpu_rom_bus_out;

`ifdef junk
// Note that path for INIT_FILE is relative to diamond implementation directory.
bus_rom #(.BUS_ADDR(32'h0010_0000), .LOGSIZE(16), .INIT_FILE("../../fw/tof.mem")) cpu_rom
  (
  .bus_in (cpu_bus_in),
  .bus_out (cpu_rom_bus_out)
  );
`endif

reg mclk_oe;
reg mclk_oe_1;

// Lattice way of accessing spi_sclk pin
USRMCLK u1 (.USRMCLKI(flash_clk), .USRMCLKTS(mclk_oe)) /* synthesis syn_noprune=1 */;

always @(posedge cpu_clk)
  if (!reset_l)
    begin
      mclk_oe_1 <= 1;
      mclk_oe <= 1;
    end
  else
    begin
      mclk_oe_1 <= 0;
      mclk_oe <= mclk_oe_1;
    end

bus_spiflash #(
  .BUS_ADDR_MEM(32'h0010_0000),
  .SIZE_MEM(32'h0010_0000),
  .BUS_ADDR_CFG(32'h0200_0000),
  .BANK0_OFFSET(32'h0060_0000),
  .BANK1_OFFSET(32'h00E0_0000)
) cpu_rom (
  .bus_in (cpu_bus_in),
  .bus_out (cpu_rom_bus_out),

  .flash_cs_l (flash_cs_l),
  .flash_clk (flash_clk),

  .flash_io0_oe (flash_oe0),
  .flash_io1_oe (flash_oe1),
  .flash_io2_oe (flash_oe2),
  .flash_io3_oe (flash_oe3),

  .flash_io0_do (flash_do0),
  .flash_io1_do (flash_do1),
  .flash_io2_do (flash_do2),
  .flash_io3_do (flash_do3),

  .flash_io0_di (flash_di0),
  .flash_io1_di (flash_di1),
  .flash_io2_di (flash_di2),
  .flash_io3_di (flash_di3)
  );

// UART

wire [BUS_OUT_WIDTH-1:0] uart_bus_out;

bus_uart #(.BUS_ADDR(32'h0200_0004), .CPU_FREQ(CPU_FREQ)) uart
  (
  .bus_in (cpu_bus_in),
  .bus_out (uart_bus_out),
  .ser_tx (uart_tx),
  .ser_rx (uart_rx),
  .recv_buf_valid (uart_irq)
  );

// Software controllable LEDs register

wire [BUS_OUT_WIDTH-1:0] led_reg_bus_out;

wire [31:0] led_reg_out;

bus_reg #(.BUS_ADDR(32'h0200_000c)) led_reg
  (
  .bus_in (cpu_bus_in),
  .bus_out (led_reg_bus_out),

  .out (led_reg_out)
  );

// Software access to CPU_FREQ

wire [BUS_OUT_WIDTH-1:0] cpu_reg_10_bus_out;

bus_ro_reg #(.BUS_ADDR(32'h0200_0010)) cpu_reg_10
  (
  .bus_in (cpu_bus_in),
  .bus_out (cpu_reg_10_bus_out),

  .in (CPU_FREQ)
  );

// Free running timer

reg [31:0] wallclock;

always @(posedge cpu_clk)
  if (!reset_l)
    wallclock <= 0;
  else
    wallclock <= wallclock + 1'd1;

wire [BUS_OUT_WIDTH-1:0] cpu_reg_14_bus_out;

bus_ro_reg #(.BUS_ADDR(32'h0200_0014)) cpu_reg_14
  (
  .bus_in (cpu_bus_in),
  .bus_out (cpu_reg_14_bus_out),

  .in (wallclock)
  );

// Bridge to 125 MHz clock domain

wire [BUS_OUT_WIDTH-1:0] peripheral_bus_out;

bus_window_async #(.BUS_ADDR(32'h0300_0000), .ADDRWIDTH(12)) peripheral_bus
  (
  .sub_clk (bus_clk),
  .sub_reset_l (reset_l),

  .bus_in (cpu_bus_in),
  .bus_out (peripheral_bus_out),

  .sub_bus_in (bus_in),
  .sub_bus_out (bus_out)
  );

assign cpu_bus_out =
  cpu_ram_bus_out |
  cpu_rom_bus_out |
  uart_bus_out |
  led_reg_bus_out |
  cpu_reg_10_bus_out |
  cpu_reg_14_bus_out |
  peripheral_bus_out;

endmodule
