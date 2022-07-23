// SDRAM interface

module sdram
  (
  clk,
  clk270,
  reset_l,

  sdram_dq,
  sdram_a,
  sdram_ba,
  sdram_cke,
  sdram_clk,
  sdram_cs_l,
  sdram_cas_l,
  sdram_ras_l,
  sdram_we_l,
  sdram_ldqm,
  sdram_udqm,

  ack, // Assert high to request a transaction
  req, // Pulses high when transactions accepted
  wr, // High for write
  addr, // Address
  wr_data, // Write data

  rd_ack, // Pulses high when rd_data has result of read transaction
  rd_data
  );

input clk;
input clk270;
input reset_l;

inout [15:0] sdram_dq;
output [12:0] sdram_a;
output [1:0] sdram_ba;
output sdram_cke;
output sdram_clk;
output sdram_cs_l;
output sdram_ras_l;
output sdram_cas_l;
output sdram_we_l;
output sdram_ldqm;
output sdram_udqm;

output ack;
reg ack, ns_ack;

input req;
input wr;
//input [22:0] addr; // row[12:0], col[9:1], ba[1:0]
input [21:0] addr; // row[12:0], col[8:1], ba[1:0]
input [63:0] wr_data;

output rd_ack;
reg rd_ack, ns_rd_ack;
output [63:0] rd_data;
reg [63:0] rd_data, ns_rd_data;

// We are instantiating primitives here because it's difficult to get synplify to
// to infer INFFs and OUTFFs.

// Clock

ODDRX1F clk_out (.SCLK(clk270), .RST(1'd0), .D0(1'd1), .D1(1'd0), .Q(sdram_clk));

// Data bits

wire [15:0] d;
wire [15:0] t;
wire [15:0] q;

reg [15:0] ns_t;
reg [15:0] ns_q;
wire [15:0] ns_d;

BB d_obuf_0(.I(q[0]), .T(t[0]), .B(sdram_dq[0]), .O(ns_d[0]));
IFS1P3DX d_inff_0(.D(ns_d[0]), .SCLK(clk), .Q(d[0]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_0(.D(ns_t[0]), .SCLK(clk), .Q(t[0]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_0(.D(ns_q[0]), .SCLK(clk), .Q(q[0]), .CD(1'd0), .SP(1'd1));

BB d_obuf_1(.I(q[1]), .T(t[1]), .B(sdram_dq[1]), .O(ns_d[1]));
IFS1P3DX d_inff_1(.D(ns_d[1]), .SCLK(clk), .Q(d[1]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_1(.D(ns_t[1]), .SCLK(clk), .Q(t[1]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_1(.D(ns_q[1]), .SCLK(clk), .Q(q[1]), .CD(1'd0), .SP(1'd1));

BB d_obuf_2(.I(q[2]), .T(t[2]), .B(sdram_dq[2]), .O(ns_d[2]));
IFS1P3DX d_inff_2(.D(ns_d[2]), .SCLK(clk), .Q(d[2]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_2(.D(ns_t[2]), .SCLK(clk), .Q(t[2]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_2(.D(ns_q[2]), .SCLK(clk), .Q(q[2]), .CD(1'd0), .SP(1'd1));

BB d_obuf_3(.I(q[3]), .T(t[3]), .B(sdram_dq[3]), .O(ns_d[3]));
IFS1P3DX d_inff_3(.D(ns_d[3]), .SCLK(clk), .Q(d[3]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_3(.D(ns_t[3]), .SCLK(clk), .Q(t[3]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_3(.D(ns_q[3]), .SCLK(clk), .Q(q[3]), .CD(1'd0), .SP(1'd1));

BB d_obuf_4(.I(q[4]), .T(t[4]), .B(sdram_dq[4]), .O(ns_d[4]));
IFS1P3DX d_inff_4(.D(ns_d[4]), .SCLK(clk), .Q(d[4]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_4(.D(ns_t[4]), .SCLK(clk), .Q(t[4]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_4(.D(ns_q[4]), .SCLK(clk), .Q(q[4]), .CD(1'd0), .SP(1'd1));

BB d_obuf_5(.I(q[5]), .T(t[5]), .B(sdram_dq[5]), .O(ns_d[5]));
IFS1P3DX d_inff_5(.D(ns_d[5]), .SCLK(clk), .Q(d[5]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_5(.D(ns_t[5]), .SCLK(clk), .Q(t[5]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_5(.D(ns_q[5]), .SCLK(clk), .Q(q[5]), .CD(1'd0), .SP(1'd1));

BB d_obuf_6(.I(q[6]), .T(t[6]), .B(sdram_dq[6]), .O(ns_d[6]));
IFS1P3DX d_inff_6(.D(ns_d[6]), .SCLK(clk), .Q(d[6]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_6(.D(ns_t[6]), .SCLK(clk), .Q(t[6]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_6(.D(ns_q[6]), .SCLK(clk), .Q(q[6]), .CD(1'd0), .SP(1'd1));

BB d_obuf_7(.I(q[7]), .T(t[7]), .B(sdram_dq[7]), .O(ns_d[7]));
IFS1P3DX d_inff_7(.D(ns_d[7]), .SCLK(clk), .Q(d[7]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_7(.D(ns_t[7]), .SCLK(clk), .Q(t[7]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_7(.D(ns_q[7]), .SCLK(clk), .Q(q[7]), .CD(1'd0), .SP(1'd1));

BB d_obuf_8(.I(q[8]), .T(t[8]), .B(sdram_dq[8]), .O(ns_d[8]));
IFS1P3DX d_inff_8(.D(ns_d[8]), .SCLK(clk), .Q(d[8]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_8(.D(ns_t[8]), .SCLK(clk), .Q(t[8]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_8(.D(ns_q[8]), .SCLK(clk), .Q(q[8]), .CD(1'd0), .SP(1'd1));

BB d_obuf_9(.I(q[9]), .T(t[9]), .B(sdram_dq[9]), .O(ns_d[9]));
IFS1P3DX d_inff_9(.D(ns_d[9]), .SCLK(clk), .Q(d[9]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_9(.D(ns_t[9]), .SCLK(clk), .Q(t[9]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_9(.D(ns_q[9]), .SCLK(clk), .Q(q[9]), .CD(1'd0), .SP(1'd1));

BB d_obuf_10(.I(q[10]), .T(t[10]), .B(sdram_dq[10]), .O(ns_d[10]));
IFS1P3DX d_inff_10(.D(ns_d[10]), .SCLK(clk), .Q(d[10]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_10(.D(ns_t[10]), .SCLK(clk), .Q(t[10]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_10(.D(ns_q[10]), .SCLK(clk), .Q(q[10]), .CD(1'd0), .SP(1'd1));

BB d_obuf_11(.I(q[11]), .T(t[11]), .B(sdram_dq[11]), .O(ns_d[11]));
IFS1P3DX d_inff_11(.D(ns_d[11]), .SCLK(clk), .Q(d[11]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_11(.D(ns_t[11]), .SCLK(clk), .Q(t[11]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_11(.D(ns_q[11]), .SCLK(clk), .Q(q[11]), .CD(1'd0), .SP(1'd1));

BB d_obuf_12(.I(q[12]), .T(t[12]), .B(sdram_dq[12]), .O(ns_d[12]));
IFS1P3DX d_inff_12(.D(ns_d[12]), .SCLK(clk), .Q(d[12]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_12(.D(ns_t[12]), .SCLK(clk), .Q(t[12]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_12(.D(ns_q[12]), .SCLK(clk), .Q(q[12]), .CD(1'd0), .SP(1'd1));

BB d_obuf_13(.I(q[13]), .T(t[13]), .B(sdram_dq[13]), .O(ns_d[13]));
IFS1P3DX d_inff_13(.D(ns_d[13]), .SCLK(clk), .Q(d[13]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_13(.D(ns_t[13]), .SCLK(clk), .Q(t[13]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_13(.D(ns_q[13]), .SCLK(clk), .Q(q[13]), .CD(1'd0), .SP(1'd1));

BB d_obuf_14(.I(q[14]), .T(t[14]), .B(sdram_dq[14]), .O(ns_d[14]));
IFS1P3DX d_inff_14(.D(ns_d[14]), .SCLK(clk), .Q(d[14]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_14(.D(ns_t[14]), .SCLK(clk), .Q(t[14]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_14(.D(ns_q[14]), .SCLK(clk), .Q(q[14]), .CD(1'd0), .SP(1'd1));

BB d_obuf_15(.I(q[15]), .T(t[15]), .B(sdram_dq[15]), .O(ns_d[15]));
IFS1P3DX d_inff_15(.D(ns_d[15]), .SCLK(clk), .Q(d[15]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_tff_15(.D(ns_t[15]), .SCLK(clk), .Q(t[15]), .CD(1'b0), .SP(1'd1));
OFS1P3DX d_off_15(.D(ns_q[15]), .SCLK(clk), .Q(q[15]), .CD(1'd0), .SP(1'd1));

// Address bits

wire [12:0] a;
reg [12:0] ns_a;
reg [12:0] a_hold;

OB a_obuf_0(.I(a[0]), .O(sdram_a[0]));
OFS1P3DX a_off_0(.D(ns_a[0]), .SCLK(clk), .Q(a[0]), .CD(1'd0), .SP(1'd1));

OB a_obuf_1(.I(a[1]), .O(sdram_a[1]));
OFS1P3DX a_off_1(.D(ns_a[1]), .SCLK(clk), .Q(a[1]), .CD(1'd0), .SP(1'd1));

OB a_obuf_2(.I(a[2]), .O(sdram_a[2]));
OFS1P3DX a_off_2(.D(ns_a[2]), .SCLK(clk), .Q(a[2]), .CD(1'd0), .SP(1'd1));

OB a_obuf_3(.I(a[3]), .O(sdram_a[3]));
OFS1P3DX a_off_3(.D(ns_a[3]), .SCLK(clk), .Q(a[3]), .CD(1'd0), .SP(1'd1));

OB a_obuf_4(.I(a[4]), .O(sdram_a[4]));
OFS1P3DX a_off_4(.D(ns_a[4]), .SCLK(clk), .Q(a[4]), .CD(1'd0), .SP(1'd1));

OB a_obuf_5(.I(a[5]), .O(sdram_a[5]));
OFS1P3DX a_off_5(.D(ns_a[5]), .SCLK(clk), .Q(a[5]), .CD(1'd0), .SP(1'd1));

OB a_obuf_6(.I(a[6]), .O(sdram_a[6]));
OFS1P3DX a_off_6(.D(ns_a[6]), .SCLK(clk), .Q(a[6]), .CD(1'd0), .SP(1'd1));

OB a_obuf_7(.I(a[7]), .O(sdram_a[7]));
OFS1P3DX a_off_7(.D(ns_a[7]), .SCLK(clk), .Q(a[7]), .CD(1'd0), .SP(1'd1));

OB a_obuf_8(.I(a[8]), .O(sdram_a[8]));
OFS1P3DX a_off_8(.D(ns_a[8]), .SCLK(clk), .Q(a[8]), .CD(1'd0), .SP(1'd1));

OB a_obuf_9(.I(a[9]), .O(sdram_a[9]));
OFS1P3DX a_off_9(.D(ns_a[9]), .SCLK(clk), .Q(a[9]), .CD(1'd0), .SP(1'd1));

OB a_obuf_10(.I(a[10]), .O(sdram_a[10]));
OFS1P3DX a_off_10(.D(ns_a[10]), .SCLK(clk), .Q(a[10]), .CD(1'd0), .SP(1'd1));

OB a_obuf_11(.I(a[11]), .O(sdram_a[11]));
OFS1P3DX a_off_11(.D(ns_a[11]), .SCLK(clk), .Q(a[11]), .CD(1'd0), .SP(1'd1));

OB a_obuf_12(.I(a[12]), .O(sdram_a[12]));
OFS1P3DX a_off_12(.D(ns_a[12]), .SCLK(clk), .Q(a[12]), .CD(1'd0), .SP(1'd1));

// Bank select

wire [1:0] ba;
reg [1:0] ns_ba;
reg [1:0] ba_hold;

OB ba_obuf_0(.I(ba[0]), .O(sdram_ba[0]));
OFS1P3DX ba_off_0(.D(ns_ba[0]), .SCLK(clk), .Q(ba[0]), .CD(1'd0), .SP(1'd1));

OB ba_obuf_1(.I(ba[1]), .O(sdram_ba[1]));
OFS1P3DX ba_off_1(.D(ns_ba[1]), .SCLK(clk), .Q(ba[1]), .CD(1'd0), .SP(1'd1));

// Clock enable

wire cke;
reg ns_cke;
reg cke_hold;

OB cke_obuf(.I(cke), .O(sdram_cke));
OFS1P3DX cke_off(.D(ns_cke), .SCLK(clk), .Q(cke), .CD(1'd0), .SP(1'd1));

// Chip select

wire cs_l;
reg ns_cs_l;

OB cs_obuf(.I(cs_l), .O(sdram_cs_l));
OFS1P3DX cs_off(.D(ns_cs_l), .SCLK(clk), .Q(cs_l), .CD(1'd0), .SP(1'd1));

// RAS

wire ras_l;
reg ns_ras_l;

OB ras_obuf(.I(ras_l), .O(sdram_ras_l));
OFS1P3DX ras_off(.D(ns_ras_l), .SCLK(clk), .Q(ras_l), .CD(1'd0), .SP(1'd1));

// CAS

wire cas_l;
reg ns_cas_l;

OB cas_obuf(.I(cas_l), .O(sdram_cas_l));
OFS1P3DX cas_off(.D(ns_cas_l), .SCLK(clk), .Q(cas_l), .CD(1'd0), .SP(1'd1));

// Write enable

wire we_l;
reg ns_we_l;

OB we_obuf(.I(we_l), .O(sdram_we_l));
OFS1P3DX we_off(.D(ns_we_l), .SCLK(clk), .Q(we_l), .CD(1'd0), .SP(1'd1));

// Lower byte mask

wire ldqm;
reg ns_ldqm;
reg ldqm_hold;

OB ldqm_obuf(.I(ldqm), .O(sdram_ldqm));
OFS1P3DX ldqm_off(.D(ns_ldqm), .SCLK(clk), .Q(ldqm), .CD(1'd0), .SP(1'd1));

// Upper byte mask

wire udqm;
reg ns_udqm;
reg udqm_hold;

OB udqm_obuf(.I(udqm), .O(sdram_udqm));
OFS1P3DX udqm_off(.D(ns_udqm), .SCLK(clk), .Q(udqm), .CD(1'd0), .SP(1'd1));

// State machine

reg [3:0] state, ns_state;
reg [15:0] count, ns_count;
reg [11:0] refresh_counter, ns_refresh_counter;
reg [1:0] shift_count, ns_shift_count;
reg [47:0] shift_out, ns_shift_out;
reg [47:0] rd_shift, ns_rd_shift;

reg rd_ack_8, ns_rd_ack_8;
reg rd_ack_7, ns_rd_ack_7;
reg rd_ack_6, ns_rd_ack_6;
reg rd_ack_5, ns_rd_ack_5;
reg rd_ack_4, ns_rd_ack_4;
reg rd_ack_3, ns_rd_ack_3;
reg rd_ack_2, ns_rd_ack_2;
reg rd_ack_1, ns_rd_ack_1;

reg req_hold, ns_req_hold;

parameter
  RESET = 0,
  STARTUP = 1,
  MODE = 2,
  IDLE = 3,
  CAS = 4,
  PRE = 5
  ;

parameter tCYCLE = 8; // ns per cycle
parameter BURST_COUNT = 4; // Burst size we are using
parameter CL_COUNT = 2; // CAS latency setting

// Timing parameters from datasheet: round up to number of cycles
parameter tRC_COUNT = ((127 + tCYCLE - 1) / tCYCLE);
parameter tRP_COUNT = ((21 + tCYCLE - 1) / tCYCLE);
parameter tRCD_COUNT = ((21 + tCYCLE - 1) / tCYCLE);
parameter tMRD_COUNT = ((14 + tCYCLE - 1) / tCYCLE);
parameter tREFI_COUNT = ((7800 + tCYCLE - 1) / tCYCLE);
parameter tWR_COUNT = ((14 + tCYCLE - 1) / tCYCLE);
parameter tWRITECASPRE = BURST_COUNT - 1 + tWR_COUNT; // (Burst length - 1) + tWR_COUNT
parameter tREADCASPRE = BURST_COUNT; // (Burst length)

always @(posedge clk)
  if (!reset_l)
    begin
      state <= RESET;
      count <= 16'hffff; // Long period of stable signels before we enable clock
      refresh_counter <= 0;
      shift_out <= 0;
      shift_count <= 0;
      rd_data <= 0;
      rd_ack_8 <= 0;
      rd_ack_7 <= 0;
      rd_ack_6 <= 0;
      rd_ack_5 <= 0;
      rd_ack_4 <= 0;
      rd_ack_3 <= 0;
      rd_ack_1 <= 0;
      rd_ack <= 0;
      req_hold <= 0;
      rd_shift <= 0;
      a_hold <= 0;
      ba_hold <= 0;
      cke_hold <= 0;
      ldqm_hold <= 0;
      udqm_hold <= 0;
      ack <= 0;
    end
  else
    begin
      state <= ns_state;
      count <= ns_count;
      refresh_counter <= ns_refresh_counter;
      shift_out <= ns_shift_out;
      shift_count <= ns_shift_count;
      rd_data <= ns_rd_data;
      rd_ack_8 <= ns_rd_ack_8;
      rd_ack_7 <= ns_rd_ack_7;
      rd_ack_6 <= ns_rd_ack_6;
      rd_ack_5 <= ns_rd_ack_5;
      rd_ack_4 <= ns_rd_ack_4;
      rd_ack_3 <= ns_rd_ack_3;
      rd_ack_2 <= ns_rd_ack_2;
      rd_ack_1 <= ns_rd_ack_1;
      rd_ack <= ns_rd_ack;
      req_hold <= ns_req_hold;
      rd_shift <= ns_rd_shift;
      a_hold <= ns_a;
      ba_hold <= ns_ba;
      cke_hold <= ns_cke;
      ldqm_hold <= ns_ldqm;
      udqm_hold <= ns_udqm;
      ack <= ns_ack;
    end


always @(*)
  begin
    ns_cs_l = 1;
    ns_ras_l = 1;
    ns_cas_l = 1;
    ns_we_l = 1;
    ns_ack = 0;
    ns_a = a_hold;
    ns_cke = cke_hold;
    ns_ldqm = ldqm_hold;
    ns_udqm = udqm_hold;
    ns_state = state;
    ns_count = count;
    ns_refresh_counter = refresh_counter;
    ns_shift_out = shift_out;
    ns_shift_count = shift_count;
    ns_rd_data = rd_data;
    ns_rd_ack_8 = rd_ack_8;
    ns_rd_ack_7 = rd_ack_7;
    ns_rd_ack_6 = rd_ack_6;
    ns_rd_ack_5 = rd_ack_5;
    ns_rd_ack_4 = rd_ack_4;
    ns_rd_ack_3 = rd_ack_3;
    ns_rd_ack_2 = rd_ack_2;
    ns_rd_ack_1 = rd_ack_1;
    ns_rd_ack = rd_ack;
    ns_req_hold = req_hold;
    ns_rd_shift = rd_shift;

    if (req)
      ns_req_hold = 1;

    // Read pipe
    ns_rd_shift = { rd_shift[47:0], d };

    ns_rd_ack_8 = 0;
    ns_rd_ack_7 = rd_ack_8;
    ns_rd_ack_6 = rd_ack_7;
    ns_rd_ack_5 = rd_ack_6;
    ns_rd_ack_4 = rd_ack_5;
    ns_rd_ack_3 = rd_ack_4;
    ns_rd_ack_2 = rd_ack_3;
    ns_rd_ack_1 = rd_ack_2;
    ns_rd_ack = rd_ack_1;

    if (rd_ack_1)
      begin
        ns_rd_data = { rd_shift[47:0], d };
      end

    ns_q = shift_out[47:32];
    ns_shift_out[47:32] = shift_out[31:16];
    ns_shift_out[31:16] = shift_out[15:0];

    if (shift_count)
      begin
        ns_t = 0;
        ns_shift_count = shift_count - 1'd1;
      end
    else
      begin
        ns_t = 16'hffff;
      end

    if (count)
      ns_count = count - 1'd1;

    ns_refresh_counter = refresh_counter - 1'd1;

    case (state)
      RESET:
        if (!count)
          begin
            ns_cke = 1;
            ns_state = STARTUP;
          end

      STARTUP:
        begin
          // Precharge all command
          ns_cs_l = 0;
          ns_ras_l = 0;
          ns_cas_l = 1;
          ns_we_l = 0;
          ns_a[10] = 1;
          ns_state = MODE;
          ns_count = tRP_COUNT - 1;
        end

      MODE:
        if (!count)
          begin
            // Set mode register
            ns_cs_l = 0;
            ns_ras_l = 0;
            ns_cas_l = 0;
            ns_we_l = 0;
            ns_ba = 0;

            ns_a[2:0] = 2; // Burst length = 4
            ns_a[3] = 0; // Burst type = sequential
            ns_a[6:4] = 3; // CAS latency = 2
            ns_a[8:7] = 0; // No test mode
            ns_a[9] = 0; // Write burst
            ns_a[12:10] = 0; // Unused

            ns_count = tMRD_COUNT - 1;
            ns_state = IDLE;
          end

      IDLE:
        if (!count)
          begin
            if (refresh_counter[11])
              begin
                ns_refresh_counter = refresh_counter + tREFI_COUNT;
                // We can not issue auto-refresh with any open banks
                ns_cs_l = 0;
                ns_ras_l = 0;
                ns_cas_l = 0;
                ns_we_l = 1;
                ns_count = tRC_COUNT - 1; // Must wait this long before next command
              end
            else if (req || req_hold)
              begin
                // RAS / activate
                ns_cs_l = 0;
                ns_ras_l = 0;
                ns_cas_l = 1;
                ns_we_l = 1;
//                ns_a[12:0] = addr[22:10]; // Row
                ns_a[12:0] = addr[21:9]; // Row
                ns_ba[1:0] = addr[1:0]; // Bank
                ns_state = CAS;
                ns_count = tRCD_COUNT - 1;
              end
          end

      CAS:
        if (!count)
          begin
            // CAS: read or write command
            ns_ack = 1;
            ns_req_hold = 0;
            ns_cs_l = 0;
            ns_ras_l = 1;
            ns_cas_l = 0;
//            ns_a[9:2] = addr[9:2]; // Column
            ns_a[9] = 0;
            ns_a[8:2] = addr[8:2]; // Column
            ns_a[1:0] = 0;
            ns_q = wr_data[63:48];
            ns_shift_out = wr_data[47:0];
            ns_a[10] = 0; // No auto-precharge
            ns_a[12:11] = 0;
            ns_state = PRE;
            if (wr)
              begin
                ns_we_l = 0;
                ns_t = 0;
                ns_shift_count = 3;
                ns_count = tWRITECASPRE - 1;
              end
            else
              begin
                ns_we_l = 1;
                ns_count = tREADCASPRE - 1;
                ns_rd_ack_6 = 1; // Latency
              end
          end

        PRE:
          if (!count)
            begin
              // Precharge, close row
              ns_cs_l = 0;
              ns_ras_l = 0;
              ns_we_l = 0;
              ns_cas_l = 1;
              ns_a[10] = 0;
              ns_count = tRP_COUNT - 1;
              ns_state = IDLE;
            end

    endcase
  end

endmodule
