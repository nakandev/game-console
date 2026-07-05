module vpu_core
  import gameconsole_pkg::*;
(
  input  wire                   clk,
  input  wire                   rst_n,

  output wire                   bg_param_en,
  output wire [PRM_ADDR_W-1:0]  bg_param_addr,
  input  wire [PRM_DATA_W-1:0]  bg_param_dout,

  output wire                   bg_map_en,
  output wire [MAP_ADDR_W-1:0]  bg_map_addr,
  input  wire [MAP_DATA_W-1:0]  bg_map_dout,

  output wire                   bg_tile_en,
  output wire [TILE_ADDR_W-1:0] bg_tile_addr,
  input  wire [TILE_DATA_W-1:0] bg_tile_dout,

  output wire                   bg_pal_en,
  output wire [PAL_ADDR_W-1:0]  bg_pal_addr,
  input  wire [PAL_DATA_W-1:0]  bg_pal_dout,

  output wire                   sp_param_en,
  output wire [PRM_ADDR_W-1:0]  sp_param_addr,
  input  wire [PRM_DATA_W-1:0]  sp_param_dout,

  output wire                   sp_tile_en,
  output wire [TILE_ADDR_W-1:0] sp_tile_addr,
  input  wire [TILE_DATA_W-1:0] sp_tile_dout,

  output wire                   sp_pal_en,
  output wire [PAL_ADDR_W-1:0]  sp_pal_addr,
  input  wire [PAL_DATA_W-1:0]  sp_pal_dout,

  output wire                   dot_clk,
  output reg  [31:0]            color,
  output wire                   hdraw,
  output wire                   vdraw
);

wire                       line_init;
wire [LINEBUFF_BANK_W-1:0] line_banka;
wire                       line_wea;
wire [LINEBUFF_ADDR_W-1:0] line_addra;
wire [LINEBUFF_DATA_W-1:0] line_dina;
wire [LINEBUFF_DATA_W-1:0] line_douta;
wire                       line_web;
wire [LINEBUFF_BANK_W-1:0] line_bankb;
wire [LINEBUFF_ADDR_W-1:0] line_addrb;
wire [LINEBUFF_DATA_W-1:0] line_dinb;
wire [LINEBUFF_DATA_W-1:0] line_doutb;

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam LMAX = HMAX * 4;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;

reg [8:0]  y;
reg [10:0] line_cycle;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    y <= 0;
    line_cycle <= 0;
  end
  else begin
    if (line_cycle < LMAX-1) begin
      line_cycle <= line_cycle + 1;
    end
    else begin
      line_cycle <= 0;
      if (y < VMAX-1) begin
        y <= y + 1;
      end
      else begin
        y <= 0;
      end
    end
  end
end

wire hdraw0;
wire vdraw0;

vpu_bg vpu_bg
(
  .clk        (clk),
  .rst_n      (rst_n),

  .line_cycle (line_cycle),
  .y          (y),

  .param_en   (bg_param_en),
  .param_addr (bg_param_addr),
  .param_dout (bg_param_dout),

  .map_en     (bg_map_en),
  .map_addr   (bg_map_addr),
  .map_dout   (bg_map_dout),

  .tile_en    (bg_tile_en),
  .tile_addr  (bg_tile_addr),
  .tile_dout  (bg_tile_dout),

  .pal_en     (bg_pal_en),
  .pal_addr   (bg_pal_addr),
  .pal_dout   (bg_pal_dout),

  .line_wea   (line_wea),
  .line_addra (line_addra),
  .line_dina  (line_dina ),
  .line_douta (line_douta),

  .dot_clk    (dot_clk),
  .color      (color),
  .hdraw      (hdraw0),
  .vdraw      (vdraw0)
);

vpu_sp vpu_sp
(
  .clk        (clk),
  .rst_n      (rst_n),

  .line_cycle (line_cycle),
  .y          (y),

  .param_en   (sp_param_en  ),
  .param_addr (sp_param_addr),
  .param_dout (sp_param_dout),

  .tile_en    (sp_tile_en  ),
  .tile_addr  (sp_tile_addr),
  .tile_dout  (sp_tile_dout),

  .pal_en     (sp_pal_en  ),
  .pal_addr   (sp_pal_addr),
  .pal_dout   (sp_pal_dout),

  .line_init  (line_init),
  .line_web   (line_web),
  .line_bankb (line_bankb),
  .line_addrb (line_addrb),
  .line_dinb  (line_dinb ),
  .line_doutb (line_doutb)
);

vpu_linebuffer vpu_linebuffer
(
  .clk   (clk),
  .rst_n (rst_n),

  .init  (line_init ),
  // .banka (line_banka),
  .wea   (line_wea  ),
  .addra (line_addra),
  .dina  (line_dina ),
  .douta (line_douta),
  .web   (line_web  ),
  .bankb (line_bankb),
  .addrb (line_addrb),
  .dinb  (line_dinb ),
  .doutb (line_doutb)
);

// assign hdraw = (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
// assign vdraw = y < SCREEN_H;
assign hdraw = hdraw0;
assign vdraw = vdraw0;
// localparam VH_DELAY = 16;
// logic [0:0] vdelay[VH_DELAY];
// logic [0:0] hdelay[VH_DELAY];
// always_ff @(posedge clk) begin
//   // hdelay[0] <= (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
//   // vdelay[0] <= y < SCREEN_H;
//   hdelay[0] <= hdraw0;
//   vdelay[0] <= vdraw0;
//   for (int i=1; i<VH_DELAY; i++) begin
//     hdelay[i] <= hdelay[i-1];
//     vdelay[i] <= vdelay[i-1];
//   end
//   // hdraw <= hdelay[10-1];
//   // vdraw <= vdelay[10-1];
//   hdraw <= hdelay[12-1];
//   vdraw <= vdelay[12-1];
// end

endmodule

module vpu_linebuffer
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire                       init,
  // input  wire [LINEBUFF_BANK_W-1:0] banka,
  input  wire                       wea,
  input  wire [LINEBUFF_ADDR_W-1:0] addra,
  input  wire [LINEBUFF_DATA_W-1:0] dina,
  output wire [LINEBUFF_DATA_W-1:0] douta,
  input  wire                       web,
  input  wire [LINEBUFF_BANK_W-1:0] bankb,
  input  wire [LINEBUFF_ADDR_W-1:0] addrb,
  input  wire [LINEBUFF_DATA_W-1:0] dinb,
  output wire [LINEBUFF_DATA_W-1:0] doutb
);

wire [0:0] banka;
wire [LINEBUFF_DATA_W-1:0] douta0;
wire [LINEBUFF_DATA_W-1:0] douta1;
wire [LINEBUFF_DATA_W-1:0] doutb0;
wire [LINEBUFF_DATA_W-1:0] doutb1;

assign banka = ~bankb;
assign douta = (banka==0) ? douta0 : douta1;
assign doutb = (bankb==0) ? doutb0 : doutb1;

// linebuffer_wrapper linebuffer_ram0 (
//   .BRAM_PORTA_0_addr(addra),
//   .BRAM_PORTA_0_clk (clk),
//   .BRAM_PORTA_0_din (dina),
//   .BRAM_PORTA_0_dout(douta0),
//   .BRAM_PORTA_0_en  (banka==0),
//   .BRAM_PORTA_0_we  (wea),
//   .BRAM_PORTA_1_addr(addrb),
//   .BRAM_PORTA_1_clk (clk),
//   .BRAM_PORTA_1_din (dinb),
//   .BRAM_PORTA_1_dout(doutb0),
//   .BRAM_PORTA_1_en  (bankb==0),
//   .BRAM_PORTA_1_we  (web)
// );
// 
// linebuffer_wrapper linebuffer_ram1 (
//   .BRAM_PORTA_0_addr(addra),
//   .BRAM_PORTA_0_clk (clk),
//   .BRAM_PORTA_0_din (dina),
//   .BRAM_PORTA_0_dout(douta1),
//   .BRAM_PORTA_0_en  (banka==1),
//   .BRAM_PORTA_0_we  (wea),
//   .BRAM_PORTA_1_addr(addrb),
//   .BRAM_PORTA_1_clk (clk),
//   .BRAM_PORTA_1_din (dinb),
//   .BRAM_PORTA_1_dout(doutb1),
//   .BRAM_PORTA_1_en  (bankb==1),
//   .BRAM_PORTA_1_we  (web)
// );

bram_tdp_rf_rf #(
  .ADDR_W(LINEBUFF_ADDR_W),
  .DATA_W(LINEBUFF_DATA_W)
) linebuffer_ram0 (
  .clka (clk),
  .ena  (banka==0),
  .wea  (wea  ),
  .addra(addra),
  .dina (dina ),
  .douta(douta0),
  .clkb (clk),
  .enb  (bankb==0),
  .web  (web  ),
  .addrb(addrb),
  .dinb (dinb ),
  .doutb(doutb0)
);

bram_tdp_rf_rf #(
  .ADDR_W(LINEBUFF_ADDR_W),
  .DATA_W(LINEBUFF_DATA_W)
) linebuffer_ram1 (
  .clka (clk),
  .ena  (banka==1),
  .wea  (wea  ),
  .addra(addra),
  .dina (dina ),
  .douta(douta1),
  .clkb (clk),
  .enb  (bankb==1),
  .web  (web  ),
  .addrb(addrb),
  .dinb (dinb ),
  .doutb(doutb1)
);

endmodule
