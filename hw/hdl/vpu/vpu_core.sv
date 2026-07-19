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
wire [LINEBUFF_BANK_W-1:0] line_bank;
wire                       sp_line_ena;
wire                       sp_line_wea;
wire [LINEBUFF_ADDR_W-1:0] sp_line_addra;
wire [LINEBUFF_DATA_W-1:0] sp_line_dina;
wire [LINEBUFF_DATA_W-1:0] sp_line_douta;
wire                       sp_line_enb;
wire                       sp_line_web;
wire [LINEBUFF_ADDR_W-1:0] sp_line_addrb;
wire [LINEBUFF_DATA_W-1:0] sp_line_dinb;
wire [LINEBUFF_DATA_W-1:0] sp_line_doutb;
wire                       bg_line_ena;
wire                       bg_line_wea;
wire [LINEBUFF_ADDR_W-1:0] bg_line_addra;
wire [LINEBUFF_DATA_W-1:0] bg_line_dina;
wire [LINEBUFF_DATA_W-1:0] bg_line_douta;

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

assign line_bank = y[0]; // y & 1'b1;

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

  .line_ena   (bg_line_ena),
  .line_wea   (bg_line_wea),
  .line_addra (bg_line_addra),
  .line_dina  (bg_line_dina ),
  .line_douta (bg_line_douta),

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

  .line_ena   (sp_line_ena),
  .line_wea   (sp_line_wea),
  .line_addra (sp_line_addra),
  .line_dina  (sp_line_dina ),
  .line_douta (sp_line_douta),

  .line_enb   (sp_line_enb),
  .line_web   (sp_line_web),
  .line_addrb (sp_line_addrb),
  .line_dinb  (sp_line_dinb ),
  .line_doutb (sp_line_doutb)
);

vpu_linebuffer vpu_linebuffer
(
  .clk   (clk),
  .rst_n (rst_n),

  .init  (line_init ),
  .bank  (line_bank ),

  .sp_ena   (sp_line_ena  ),
  .sp_wea   (sp_line_wea  ),
  .sp_addra (sp_line_addra),
  .sp_dina  (sp_line_dina ),
  .sp_douta (sp_line_douta),

  .sp_enb   (sp_line_enb  ),
  .sp_web   (sp_line_web  ),
  .sp_addrb (sp_line_addrb),
  .sp_dinb  (sp_line_dinb ),
  .sp_doutb (sp_line_doutb),

  .bg_ena   (bg_line_ena  ),
  .bg_wea   (bg_line_wea  ),
  .bg_addra (bg_line_addra),
  .bg_dina  (bg_line_dina ),
  .bg_douta (bg_line_douta)
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
  input  wire [0:0]                 bank,
  input  wire                       sp_ena,
  input  wire                       sp_wea,
  input  wire [LINEBUFF_ADDR_W-1:0] sp_addra,
  input  wire [LINEBUFF_DATA_W-1:0] sp_dina,
  output wire [LINEBUFF_DATA_W-1:0] sp_douta,
  input  wire                       sp_enb,
  input  wire                       sp_web,
  input  wire [LINEBUFF_ADDR_W-1:0] sp_addrb,
  input  wire [LINEBUFF_DATA_W-1:0] sp_dinb,
  output wire [LINEBUFF_DATA_W-1:0] sp_doutb,
  input  wire                       bg_ena,
  input  wire                       bg_wea,
  input  wire [LINEBUFF_ADDR_W-1:0] bg_addra,
  input  wire [LINEBUFF_DATA_W-1:0] bg_dina,
  output wire [LINEBUFF_DATA_W-1:0] bg_douta
);
//         : y=even  | y=odd
// ram0 *a :  sp(r)  |  bg(r)
//      *b :  sp(w)  | (bg(w))
// ram1 *a :  bg(r)  |  sp(r)
//      *b : (bg(w)) |  sp(w)

wire                       ena0  , ena1  ;
wire                       wea0  , wea1  ;
wire [LINEBUFF_ADDR_W-1:0] addra0, addra1;
wire [LINEBUFF_DATA_W-1:0] dina0 , dina1 ;
wire [LINEBUFF_DATA_W-1:0] douta0, douta1;
wire                       enb0  , enb1  ;
wire                       web0  , web1  ;
wire [LINEBUFF_ADDR_W-1:0] addrb0, addrb1;
wire [LINEBUFF_DATA_W-1:0] dinb0 , dinb1 ;
wire [LINEBUFF_DATA_W-1:0] doutb0, doutb1;

assign ena0      = (bank==0) ? sp_ena   : bg_ena  ;
assign wea0      = (bank==0) ? sp_wea   : bg_wea  ;
assign addra0    = (bank==0) ? sp_addra : bg_addra;
assign dina0     = (bank==0) ? sp_dina  : bg_dina ;
assign enb0      = (bank==0) ? sp_enb   : 0       ;
assign web0      = (bank==0) ? sp_web   : 0       ;
assign addrb0    = (bank==0) ? sp_addrb : 0       ;
assign dinb0     = (bank==0) ? sp_dinb  : 0       ;

assign ena1      = (bank==0) ? bg_ena   : sp_ena  ;
assign wea1      = (bank==0) ? bg_wea   : sp_wea  ;
assign addra1    = (bank==0) ? bg_addra : sp_addra;
assign dina1     = (bank==0) ? bg_dina  : sp_dina ;
assign enb1      = (bank==0) ? 0        : sp_enb  ;
assign web1      = (bank==0) ? 0        : sp_web  ;
assign addrb1    = (bank==0) ? 0        : sp_addrb;
assign dinb1     = (bank==0) ? 0        : sp_dinb ;

assign sp_douta = (bank==0) ? douta0   : douta1;
assign bg_douta = (bank==0) ? douta1   : douta0;
assign sp_doutb = (bank==0) ? douta0   : douta1;
// assign bg_doutb = (bank==0) ? douta1 : douta0;

// linebuffer_wrapper linebuffer_ram0 (
//   .BRAM_PORTA_0_addr(addra0),
//   .BRAM_PORTA_0_clk (clk),
//   .BRAM_PORTA_0_din (dina0),
//   .BRAM_PORTA_0_dout(douta0),
//   .BRAM_PORTA_0_en  (ena0),
//   .BRAM_PORTA_0_we  (wea0),
//   .BRAM_PORTB_0_addr(addrb0),
//   .BRAM_PORTB_0_clk (clk),
//   .BRAM_PORTB_0_din (dinb0),
//   .BRAM_PORTB_0_dout(doutb0),
//   .BRAM_PORTB_0_en  (enb0),
//   .BRAM_PORTB_0_we  (web0)
// );
// 
// linebuffer_wrapper linebuffer_ram1 (
//   .BRAM_PORTA_0_addr(addra1),
//   .BRAM_PORTA_0_clk (clk),
//   .BRAM_PORTA_0_din (dina1),
//   .BRAM_PORTA_0_dout(douta1),
//   .BRAM_PORTA_0_en  (ena1),
//   .BRAM_PORTA_0_we  (wea1),
//   .BRAM_PORTB_0_addr(addrb1),
//   .BRAM_PORTB_0_clk (clk),
//   .BRAM_PORTB_0_din (dinb1),
//   .BRAM_PORTB_0_dout(doutb1),
//   .BRAM_PORTB_0_en  (enb1),
//   .BRAM_PORTB_0_we  (web1)
// );

bram_tdp_rf_rf #(
  .ADDR_W(LINEBUFF_ADDR_W),
  .DATA_W(LINEBUFF_DATA_W)
) linebuffer_ram0 (
  .clka (clk),
  .ena  (ena0  ),
  .wea  (wea0  ),
  .addra(addra0),
  .dina (dina0 ),
  .douta(douta0),
  .clkb (clk),
  .enb  (enb0  ),
  .web  (web0  ),
  .addrb(addrb0),
  .dinb (dinb0 ),
  .doutb(doutb0)
);

bram_tdp_rf_rf #(
  .ADDR_W(LINEBUFF_ADDR_W),
  .DATA_W(LINEBUFF_DATA_W)
) linebuffer_ram1 (
  .clka (clk),
  .ena  (ena1  ),
  .wea  (wea1  ),
  .addra(addra1),
  .dina (dina1 ),
  .douta(douta1),
  .clkb (clk),
  .enb  (enb1  ),
  .web  (web1  ),
  .addrb(addrb1),
  .dinb (dinb1 ),
  .doutb(doutb1)
);

endmodule
