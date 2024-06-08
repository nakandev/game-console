module vpu_core
  import gameconsole_pkg::*;
(
  input  wire                   clk,
  input  wire                   rst_n,

  output wire                   bg_param_en,
  output wire [SP_ADDR_W-1:0]   bg_param_addr,
  input  wire [SP_DATA_W-1:0]   bg_param_dout,

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
  output wire [SP_ADDR_W-1:0]   sp_param_addr,
  input  wire [SP_DATA_W-1:0]   sp_param_dout,

  output wire                   sp_tile_en,
  output wire [TILE_ADDR_W-1:0] sp_tile_addr,
  input  wire [TILE_DATA_W-1:0] sp_tile_dout,

  output wire                   sp_pal_en,
  output wire [PAL_ADDR_W-1:0]  sp_pal_addr,
  input  wire [PAL_DATA_W-1:0]  sp_pal_dout,

  output wire                   dot_clk,
  output reg  [31:0]            color,
  output wire                   hsync,
  output wire                   vsync
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

reg [8:0]  y = 0;
reg [10:0] line_cycle = 0;

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
  .hsync      (hsync),
  .vsync      (vsync)
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
reg [LINEBUFF_DATA_W-1:0] douta0;
reg [LINEBUFF_DATA_W-1:0] douta1;
reg [LINEBUFF_DATA_W-1:0] doutb0;
reg [LINEBUFF_DATA_W-1:0] doutb1;
// reg [LINEBUFF_DATA_W-1:0] linebuffer0[SCREEN_W];
// reg [LINEBUFF_DATA_W-1:0] linebuffer1[SCREEN_W];
reg lstate = 0;

assign banka = ~bankb;
assign douta = (banka==0) ? douta0 : douta1;
assign doutb = (bankb==0) ? doutb0 : doutb1;

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

// always @(posedge clk) begin
//   if (~rst_n) begin
//     lstate <= 0;
//     douta <= 0;
//     doutb <= 0;
//     for (int i=0; i<SCREEN_W; i++) begin
//       linebuffer0[i] = 0;
//       linebuffer1[i] = 0;
//     end
//   end else begin
//     if (lstate == 0) begin
//       if (init) begin
//         for (int i=0; i<SCREEN_W; i++) begin
//           if (bankb) begin
//             linebuffer1[i] = 0;
//           end else begin
//             linebuffer0[i] = 0;
//           end
//         end
//         lstate <= 1;
//       end
//     end
//     else begin
//       if (addrb < SCREEN_W) begin
//         if (bankb) begin
//           doutb <= linebuffer1[addrb];
//           if (web) begin
//             linebuffer1[addrb] <= dinb;
//           end
//         end else begin
//           doutb <= linebuffer0[addrb];
//           if (web) begin
//             linebuffer0[addrb] <= dinb;
//           end
//         end
//       end
//       if (addra < SCREEN_W) begin
//         if (banka) begin
//           douta <= linebuffer1[addra];
//           // if (wea) begin
//           //   linebuffer1[addra] <= dina;
//           // end
//         end else begin
//           douta <= linebuffer0[addra];
//           // if (wea) begin
//           //   linebuffer0[addra] <= dina;
//           // end
//         end
//       end
//     end
//   end
// end

endmodule
