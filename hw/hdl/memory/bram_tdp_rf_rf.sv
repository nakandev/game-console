module bram_tdp_rf_rf #(
  parameter ADDR_W = 8,
  parameter DATA_W = 8
)(
  clka,
  ena,
  wea,
  addra,
  dina,
  douta,
  clkb,
  enb,
  web,
  addrb,
  dinb,
  doutb
);

input  wire [ 0:0] clka,clkb,ena,enb,wea,web;
input  wire [ADDR_W-1:0] addra,addrb;
input  wire [DATA_W-1:0] dina,dinb;
output reg  [DATA_W-1:0] douta,doutb;
reg [DATA_W-1:0] ram [2**ADDR_W-1:0];
// reg [DATA_W-1:0] doutaa,doutbb;

always @(posedge clka) begin
  if (ena) begin
    if (wea) ram[addra] <= dina;
    douta <= ram[addra];
  end
end

always @(posedge clkb) begin
  if (enb) begin
    if (web) ram[addrb] <= dinb;
    doutb <= ram[addrb];
  end
end

endmodule
