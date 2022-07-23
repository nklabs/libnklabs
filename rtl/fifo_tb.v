module tb;

reg clk;
reg reset_l;
reg [7:0] wr_data;
reg we;
reg commit;
reg rollback;
wire [7:0] rd_data;
reg re;
wire ne;
wire ovf;
wire unf;

//frame_fifo_late #(.DATAWIDTH(8), .ADDRWIDTH(3)) fifo
fifo_sync_late #(.DATAWIDTH(8), .ADDRWIDTH(3)) fifo
  (
  .clk (clk),
  .reset_l (reset_l),
  .wr_data (wr_data),
  .we (we),
//  .commit (commit),
//  .rollback (rollback),
  .ovf (ovf),
  .unf (unf),
  .rd_data (rd_data),
  .re (re),
  .ne (ne)
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
    commit <= 0;
    rollback <= 0;
    wr_data <= 8'hxx;
    we <= 0;
    re <= 0;
    reset_l <= 1;

    @(posedge clk);
    @(posedge clk);
    reset_l <= 0;
    @(posedge clk);
    @(posedge clk);
    reset_l <= 1;
    @(posedge clk);
    @(posedge clk);


    // Completely fill it
    wr_data <= 1;
    we <= 1;

    @(posedge clk);
    wr_data <= 2;
    @(posedge clk);
    wr_data <= 3;
    @(posedge clk);
    wr_data <= 4;
    @(posedge clk);
    wr_data <= 5;
    @(posedge clk);
    wr_data <= 6;
    @(posedge clk);
    wr_data <= 7;
    @(posedge clk);
    wr_data <= 8;
    commit <= 1;
    @(posedge clk);
    commit <= 0;
    we <= 0;
    wr_data <= 8'hxx;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    // Read it out + underflow
    for (x = 0; x != 9; x = x + 1)
      begin
        re <= 1;
        @(posedge clk);
      end

    re <= 0;
    @(posedge clk);

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    // Overfill it
    wr_data <= 1;
    we <= 1;
    @(posedge clk);
    wr_data <= 2;
    @(posedge clk);
    wr_data <= 3;
    @(posedge clk);
    wr_data <= 4;
    @(posedge clk);
    wr_data <= 5;
    @(posedge clk);
    wr_data <= 6;
    @(posedge clk);
    wr_data <= 7;
    @(posedge clk);
    wr_data <= 8;
    @(posedge clk);
    wr_data <= 9;
    commit <= 1;
    @(posedge clk);
    commit <= 0;
    we <= 0;
    wr_data <= 8'hxx;

    for (x = 0; x != 10; x = x + 1)
      @(posedge clk);

    // Read it out + underflow
    for (x = 0; x != 9; x = x + 1)
      begin
        re <= 1;
        @(posedge clk);
      end

    re <= 0;
    @(posedge clk);

    $finish;
  end

endmodule
