module tb;

reg clk;
reg reset_l;
reg [7:0] wr_data;
reg we;

wire ser;

wire [7:0] rd_data;
wire ne;

uart #(.CPU_FREQ(32'd1000)) my_uart
  (
  .clk (clk),
  .reset_l (reset_l),

  .ser_tx (ser),
  .ser_rx (ser),

  .tx_fifo_wr_data (wr_data),
  .tx_fifo_we (we),
  .tx_fifo_full (), // Tx FIFO is 1/2 full
  .tx_fifo_ovf (), // Pulse for every byte not written because FIFO is completely full

  .rx_fifo_rd_data (rd_data),
  .rx_fifo_re (ne),
  .rx_fifo_ne (ne), // Rx FIFO is not empty
  .rx_fifo_full (), // Rx FIFO is 1/2 full
  .rx_fifo_ovf (), // Pulse for every received byte lost because FIFO is completely full

  .baud_rate (32'd500),

  .frame_error (), // Receive frame error
  .rx_en (1'd1) // High to enable receiver writing into FIFO

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
    wr_data <= 8'hxx;
    we <= 0;
    reset_l <= 1;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);

    // Write some data
    wr_data <= 8'h41;
    we <= 1;

    @(posedge clk);
    wr_data <= 8'h45;
    @(posedge clk);
    wr_data <= 8'h4c;
    @(posedge clk);
    wr_data <= 8'h4c;
    @(posedge clk);
    wr_data <= 8'h4f;
    @(posedge clk);
    we <= 0;
    wr_data <= 8'hxx;

    for (x = 0; x != 1000000; x = x + 1)
      @(posedge clk);

    $finish;
  end

endmodule
