`timescale 1ns / 1ns

module tb;

reg clk;
reg reset_l;

wire flash_clk;
wire flash_cs_l;
wire flash_d0;
wire flash_d1;
wire flash_d2;
wire flash_d3;

spiflash spiflash
  (
  .csb (flash_cs_l),
  .clk (flash_clk),
  .io0 (flash_d0), // MOSI
  .io1 (flash_d1), // MISO
  .io2 (flash_d2),
  .io3 (flash_d3)
  );

wire [23:0] spimem_addr;
wire [31:0] spimem_rdata;
wire spimem_valid;
wire spimem_ready;

wire flash_oe0;
wire flash_do0;
assign flash_d0 = flash_oe0 ? flash_do0 : 1'bz;

wire flash_oe1;
wire flash_do1;
assign flash_d1 = flash_oe1 ? flash_do1 : 1'bz;

wire flash_oe2;
wire flash_do2;
assign flash_d2 = flash_oe2 ? flash_do2 : 1'bz;

wire flash_oe3;
wire flash_do3;
assign flash_d3 = flash_oe3 ? flash_do3 : 1'bz;

spimemio spimemio
  (
  .clk (clk),
  .resetn (reset_l),

  .valid (spimem_valid),
  .ready (spimem_ready),
  .addr (spimem_addr),
  .rdata (spimem_rdata),

  .flash_csb (flash_cs_l),
  .flash_clk (flash_clk),

  .flash_io0_oe (flash_oe0),
  .flash_io1_oe (flash_oe1),
  .flash_io2_oe (flash_oe2),
  .flash_io3_oe (flash_oe3),

  .flash_io0_do (flash_do0),
  .flash_io1_do (flash_do1),
  .flash_io2_do (flash_do2),
  .flash_io3_do (flash_do3),

  .flash_io0_di (flash_d0),
  .flash_io1_di (flash_d1),
  .flash_io2_di (flash_d2),
  .flash_io3_di (flash_d3),

  .cfgreg_we (4'd0),
  .cfgreg_di (32'd0),
  .cfgreg_do ()
  );

reg client_rd_req;
wire client_rd_ack;
wire [31:0] client_rd_data;
reg [23:0] client_addr;

icache icache
  (
  .reset_l (reset_l),
  .clk (clk),

  // CPU side
  .client_rd_req (client_rd_req),
  .client_addr (client_addr), // Byte address
  .client_rd_ack (client_rd_ack),
  .client_rd_data (client_rd_data),

  // Flash side
  .spimem_addr (spimem_addr), // Byte address
  .spimem_rdata (spimem_rdata),
  .spimem_ready (spimem_ready),
  .spimem_valid (spimem_valid)
  );

always
  #5 clk <= !clk;

integer x;

initial
  begin
    $dumpvars(0);
    $dumpon;
    $display("Hi there!\n");
    clk <= 0;
    reset_l <= 1;
    client_rd_req <= 0;
    client_addr <= 0;

    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    client_rd_req <= 1;
    client_addr <= 24'h000108;
    @(posedge clk);
    client_rd_req <= 0;
    @(posedge clk);

    for (x = 0; x != 500; x = x + 1)
      @(posedge clk);
    client_rd_req <= 1;
    client_addr <= 24'h00011c;
    @(posedge clk);
    client_rd_req <= 0;
    @(posedge clk);

    for (x = 0; x != 500; x = x + 1)
      @(posedge clk);
    client_rd_req <= 1;
    client_addr <= 24'h000104;
    @(posedge clk);
    client_rd_req <= 0;
    @(posedge clk);

    for (x = 0; x != 1000; x = x + 1)
      @(posedge clk);
    client_rd_req <= 1;
    client_addr <= 24'h000124;
    @(posedge clk);
    client_rd_req <= 0;
    @(posedge clk);

    for (x = 0; x != 20000; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
