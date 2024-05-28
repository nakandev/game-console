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

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam LMAX = HMAX * 4;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;

wire [31:0] sp_linebuffer[320];
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

  .sp_linebuffer(sp_linebuffer),

  .dot_clk    (dot_clk),
  .color      (color),
  .hsync      (hsync),
  .vsync      (vsync)
);

reg         sp_dot_clk;
reg  [31:0] sp_color;
reg         sp_hsync;
reg         sp_vsync;
vpu_sp vpu_sp
(
  .clk        (clk),
  .rst_n      (rst_n),

  .line_cycle (line_cycle),
  .y          (y),

  .param_en   (sp_param_en),
  .param_addr (sp_param_addr),
  .param_dout (sp_param_dout),

  .tile_en    (sp_tile_en),
  .tile_addr  (sp_tile_addr),
  .tile_dout  (sp_tile_dout),

  .pal_en     (sp_pal_en),
  .pal_addr   (sp_pal_addr),
  .pal_dout   (sp_pal_dout),

  .sp_linebuffer(sp_linebuffer)
);

endmodule
