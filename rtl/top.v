module top
  (
  btn,
  clk_25mhz,

  led,

  uart_tx,
  uart_rx,

// synthesis translate_off
  flash_clk,
// synthesis translate_on
  flash_csn,
  flash_d0,
  flash_d1,
  flash_d2,
  flash_d3,

  sdram_dq,
  sdram_a,
  sdram_ba,
  sdram_cke,
  sdram_clk,
  sdram_csn,
  sdram_casn,
  sdram_rasn,
  sdram_wen,
  sdram_dqm

  );

`include "bus_params.v"

input [6:0] btn;
wire reset_l_in = btn[0]; // PWR button
input clk_25mhz;
output [7:0] led;

output uart_tx;
input uart_rx;

// synthesis translate_off
output flash_clk;
// synthesis translate_on
wire flash_clk;
output flash_csn;
inout flash_d0;
inout flash_d1;
inout flash_d2;
inout flash_d3;

inout [15:0] sdram_dq;
output [12:0] sdram_a;
output [1:0] sdram_ba;
output sdram_cke;
output sdram_clk;
output sdram_csn;
output sdram_rasn;
output sdram_casn;
output sdram_wen;
output [1:0] sdram_dqm;

// Clocking

wire clk; // 125 MHz
wire cpu_clk; // 25 MHz clock for CPU and SPI-flash
wire clk270;

wire clk_locked;

parameter CPU_FREQ = 25000000;

mainpll_inst mainpll
  (
  .CLKI (clk_25mhz),
  .RST (!reset_l_in),
  .CLKOP (clk),
  .CLKOS (cpu_clk),
  .CLKOS2 (),
  .CLKOS3 (clk270),
  .LOCK (clk_locked)
  );

// Reset syncronizer
// Synchronous reset is better in Xilinx for some reason.

reg reset_l = 0 /* syn_maxfan = 10 */;
reg reset_l_1 = 0 /* syn_maxfan = 10 */;
reg reset_l_2 = 0 /* syn_maxfan = 10 */;
reg reset_l_3 = 0;
reg reset_l_4;
reg reset_l_5;
reg reset_l_6;

always @(posedge cpu_clk or negedge clk_locked)
  if (!clk_locked)
    begin
      reset_l_4 <= 0;
      reset_l_5 <= 0;
      reset_l_6 <= 0;
    end
  else
    begin
      reset_l_4 <= reset_l_5;
      reset_l_5 <= reset_l_6;
      reset_l_6 <= 1;
    end

// This eliminates Vivado complaint that async signals appearing at block RAM inputs can corrupt contents

always @(posedge cpu_clk)
  begin
    reset_l <= reset_l_1;
    reset_l_1 <= reset_l_2;
    reset_l_2 <= reset_l_3;
    reset_l_3 <= reset_l_4;
  end

// Blinking LED

reg [7:0] blink;
reg [23:0] counter;
reg lblink;
reg [23:0] lcounter;

assign led[5:4] = 4'hf;
assign led[6] = clk_locked;
assign led[7] = !lblink;

always @(posedge clk_25mhz)
  if (!reset_l_in)
    begin
      lcounter <= 25000000;
      lblink <= 1;
    end
  else
    begin
      if (lcounter)
        lcounter <= lcounter - 1'd1;
      else
        begin
          lcounter <= 25000000;
          lblink <= ~lblink;
        end
    end


always @(posedge clk)
  if (!reset_l)
    begin
      counter <= 12000000;
      blink <= 8'h55;
    end
  else
    begin
      if (counter)
        counter <= counter - 1'd1;
      else
        begin
          counter <= 12000000;
          blink <= ~blink;
        end
    end

// Peripheral bus

wire [BUS_IN_WIDTH-1:0] bus_in; // Combined bus
wire [BUS_OUT_WIDTH-1:0] bus_out;

// Flash data bit drivers

wire flash_oe0;
wire flash_oe1;
wire flash_oe2;
wire flash_oe3;

wire flash_do0;
wire flash_do1;
wire flash_do2;
wire flash_do3;

assign flash_d0 = flash_oe0 ? flash_do0 : 1'bz;
assign flash_d1 = flash_oe1 ? flash_do1 : 1'bz;
assign flash_d2 = flash_oe2 ? flash_do2 : 1'bz;
assign flash_d3 = flash_oe3 ? flash_do3 : 1'bz;

wire [31:0] led_reg_out;

wire fpga_irq;

riscv #(.CPU_FREQ(CPU_FREQ)) riscv
  (
  .cpu_clk (cpu_clk),
  .bus_clk (clk),
  .reset_l (reset_l),

  .uart_rx (uart_rx),
  .uart_tx (uart_tx),

  .flash_clk (flash_clk),
  .flash_cs_l (flash_csn),

  .flash_oe0 (flash_oe0),
  .flash_oe1 (flash_oe1),
  .flash_oe2 (flash_oe2),
  .flash_oe3 (flash_oe3),

  .flash_do0 (flash_do0),
  .flash_do1 (flash_do1),
  .flash_do2 (flash_do2),
  .flash_do3 (flash_do3),

  .flash_di0 (flash_d0),
  .flash_di1 (flash_d1),
  .flash_di2 (flash_d2),
  .flash_di3 (flash_d3),

  .led_reg_out (led_reg_out),

  .net_irq (fpga_irq),

  .bus_in (bus_in),
  .bus_out (bus_out)
  );

assign led[3:0] = led_reg_out[3:0];

// Who am I register / hardware version register

wire [BUS_OUT_WIDTH-1:0] reg0_bus;

bus_ro_reg #(.BUS_ADDR(32'h0300_0000)) reg0
  (
  .bus_in (bus_in),
  .bus_out (reg0_bus),

  .in ( 32'h55AA8002 ) // AB indicates root port
  );

// Test register

wire [BUS_OUT_WIDTH-1:0] reg4_bus;

bus_reg #(.BUS_ADDR(32'h0300_0004)) reg4
  (
  .bus_in (bus_in),
  .bus_out (reg4_bus),

  .out (),
  .wr_pulse ()
  );

// Interrupt enable register

wire [BUS_OUT_WIDTH-1:0] reg8_bus;

wire [15:0] irq_enable;

bus_mask_reg #(.BUS_ADDR(32'h0300_0008)) reg8
  (
  .bus_in (bus_in),
  .bus_out (reg8_bus),

  .out (irq_enable),
  .wr_pulse ()
  );

// Interrupt register

wire [BUS_OUT_WIDTH-1:0] regc_bus;

wire net_irq = 0;

wire [15:0] irq_trig;

assign irq_trig[0] = net_irq; // This one is level sensitive
assign irq_trig[15:1] = 0;

bus_irq_reg #(.BUS_ADDR(32'h0300_000c), .DATAWIDTH(16), .LEVEL(16'h0001)) regc
  (
  .bus_in (bus_in),
  .bus_out (regc_bus),

  .enable (irq_enable),
  .trig (irq_trig),
  .irq (fpga_irq)
  );

// SDRAM interface

wire [63:0] sdram_rd_data;
wire [63:0] sdram_wr_data;
wire [22:0] sdram_addr;
wire sdram_wr;
wire sdram_req;
wire sdram_ack;
wire sdram_rd_ack;

sdram sdram
  (
  .clk (clk),
  .clk270 (clk270),
  .reset_l (reset_l),

  .sdram_dq (sdram_dq),
  .sdram_a (sdram_a),
  .sdram_ba (sdram_ba),
  .sdram_cke (sdram_cke),
  .sdram_clk (sdram_clk),
  .sdram_cs_l (sdram_csn),
  .sdram_cas_l (sdram_casn),
  .sdram_ras_l (sdram_rasn),
  .sdram_we_l (sdram_wen),
  .sdram_ldqm (sdram_dqm[0]),
  .sdram_udqm (sdram_dqm[1]),

  .ack (sdram_ack), // Hold high to request a transaction
  .req (sdram_req), // Pulses high when transactions accepted
  .wr (sdram_wr), // High for write
  .addr (sdram_addr), // Address
  .wr_data (sdram_wr_data), // Write data

  .rd_ack (sdram_rd_ack), // Pulses high when rd_data has result of read transaction
  .rd_data (sdram_rd_data)
  );

wire [BUS_OUT_WIDTH-1:0] reg80_bus;

bus_split_reg #(.BUS_ADDR(32'h0300_0080)) reg80
  (
  .bus_in (bus_in),
  .bus_out (reg80_bus),

  .in (sdram_rd_data[63:32]),
  .out (sdram_wr_data[63:32])
  );

wire [BUS_OUT_WIDTH-1:0] reg84_bus;

bus_split_reg #(.BUS_ADDR(32'h0300_0084)) reg84
  (
  .bus_in (bus_in),
  .bus_out (reg84_bus),

  .in (sdram_rd_data[31:0]),
  .out (sdram_wr_data[31:0])
  );

wire [BUS_OUT_WIDTH-1:0] reg88_bus;

wire [31:0] reg88_in;
wire [31:0] reg88_out;

assign reg88_in[23:0] = reg88_out[22:0];
assign sdram_addr = reg88_out[22:0];
assign sdram_wr = reg88_out[23];

reg sdram_rd_done;
reg sdram_req_done;

always @(posedge clk)
  if (!reset_l)
    begin
      sdram_rd_done <= 0;
      sdram_req_done <= 0;
    end
  else
    begin
      if (sdram_req)
        begin
          sdram_rd_done <= 0;
          sdram_req_done <= 0;
        end
      if (sdram_ack)
        sdram_req_done <= 1;
      if (sdram_rd_ack)
        sdram_rd_done <= 1;
    end

assign reg88_in[31] = sdram_req_done;
assign reg88_in[30] = sdram_rd_done;
assign reg88_in[29:24] = 0;

bus_split_reg #(.BUS_ADDR(32'h0300_0088)) reg88
  (
  .bus_in (bus_in),
  .bus_out (reg88_bus),

  .in (reg88_in),
  .out (reg88_out),
  .wr_pulse (sdram_req)
  );

assign bus_out =
  reg0_bus | reg4_bus | reg8_bus | regc_bus |
  reg80_bus | reg84_bus | reg88_bus ;

endmodule
