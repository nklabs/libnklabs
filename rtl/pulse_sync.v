module pulse_sync
  (
  i,
  i_clk,
  i_reset_l,
  o,
  o_clk,
  o_reset_l
  );

input i;
input i_clk;
input i_reset_l;

output o;
input o_clk;
input o_reset_l;

// Convert pulse to edge

reg inv;

always @(posedge i_clk)
  if (!i_reset_l)
    inv <= 0;
  else if (i)
    inv <= !inv;

// Detect edge

reg sync_2;
reg sync_1;
reg sync_0;

always @(posedge o_clk)
  if (!o_reset_l)
    begin
      sync_2 <= 0;
      sync_1 <= 0;
      sync_0 <= 0;
    end
  else
    begin
      sync_2 <= inv;
      sync_1 <= sync_2;
      sync_0 <= sync_1;
    end

wire o = (sync_0 != sync_1);
      
endmodule
 