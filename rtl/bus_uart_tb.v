module tb;

reg clk;
reg reset_l;

wire ser;
wire recv_buf_valid;

`include "bus_params.v"

// Peripheral bus
wire [BUS_IN_WIDTH-1:0] bus_in; // Combined bus
wire [BUS_OUT_WIDTH-1:0] bus_out;

reg [3:0] mem_be;
reg [31:0] mem_addr;
reg [31:0] mem_wdata;
reg mem_read;
reg mem_write;

assign bus_in[BUS_FIELD_BE+3:BUS_FIELD_BE] = mem_be;
assign bus_in[BUS_ADDR_END-1:BUS_ADDR_START] = mem_addr;
assign bus_in[BUS_WR_DATA_END-1:BUS_WR_DATA_START] = mem_wdata;
assign bus_in[BUS_FIELD_RD_REQ] = mem_read;
assign bus_in[BUS_FIELD_WR_REQ] = mem_write;
assign bus_in[BUS_FIELD_RESET_L] = reset_l;
assign bus_in[BUS_FIELD_CLK] = clk;

bus_uart #(.BUS_ADDR(32'h0200_0004), .CPU_FREQ(32'd1152000)) my_uart
  (
  .bus_in (bus_in),
  .bus_out (bus_out),
  .ser_tx (ser),
  .ser_rx (ser),
  .recv_buf_valid (recv_buf_valid)
  );

always
  begin
    #50 clk <= !clk;
  end

integer x;

initial
  begin
    $dumpvars(0);
    $dumpon;
    $display("Hi there!\n");
    clk <= 0;
    reset_l <= 1;
    mem_write <= 0;
    mem_read <= 0;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    // Write some data
    mem_wdata <= 32'h41;
    mem_addr <= 32'h02000008;
    mem_be <= 4'hf;
    mem_write <= 1;

    @(posedge clk);
    mem_write <= 0;
    mem_wdata <= 32'hx;

    for (x = 0; x != 100000; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
