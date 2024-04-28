
typedef struct {
  logic [1:0] bank;
  logic [7:0] no;
} PaletteInfo;

module vpu #(
  parameter SCREEN_W = 320,
  parameter SCREEN_H = 240,
  parameter SCREEN_HBLANK = 80,
  parameter SCREEN_VBLANK = 80
)(
  input  wire        clk,
  input  wire        rst_n,

  output wire        sp_en,
  output wire [ 9:0] sp_addr,
  input  wire [31:0] sp_dout,

  output wire        map_en,
  output wire [10:0] map_addr,
  input  wire [15:0] map_dout,

  output wire        tile_en,
  output wire [14:0] tile_addr,
  input  wire [ 7:0] tile_dout,

  output wire        pal_en,
  output wire [ 7:0] pal_addr,
  input  wire [31:0] pal_dout,

  output wire        dot_clk,
  output reg  [31:0] color,
  output wire        hsync,
  output wire        vsync
);

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;
localparam LINE_CYCLE_PARAM = 0;
localparam LINE_CYCLE_VISIBLE = LINE_CYCLE_PARAM + SCREEN_W * 4;
localparam LINE_CYCLE_MAX = LINE_CYCLE_PARAM + HMAX * 4;

reg [31:0] bg_data0_cache[4];
reg [31:0] bg_data1_cache[4];
reg [31:0] bg_affine0_cache[4];
reg [31:0] bg_affine1_cache[4];
reg [31:0] bg_affine2_cache[4];

reg [8:0] x = 0;
reg [8:0] y = 0;
reg [10:0] line_cycle = 0;
reg [10:0] pipeline_cycle = 0;
reg [1:0] bg_layer = 0;

reg param_done;

reg pipeline_enable;

enum {
  STATE_INIT,
  STATE_PARAM,
  STATE_PIPELINE,
  STATE_WAITSYNC
} state;


always_ff @(posedge clk) begin
  if (~rst_n) begin
    x <= 0;
    y <= 0;
    line_cycle <= 0;
    bg_layer <= 0;
    //state <= STATE_INIT;
  end
  else begin
    if (state < STATE_PIPELINE) begin
      //line_cycle <= LINE_CYCLE_VISIBLE;
      //x <= HMAX;
      line_cycle <= 0;
      x <= 0;
    end
    else begin
      if (line_cycle == LINE_CYCLE_MAX - 1) begin
        line_cycle <= 0;
        y <= (y < VMAX - 1) ? y + 1 : 0;
      end
      else begin
        line_cycle <= line_cycle + 1;
      end
      x <= line_cycle >> 2;
    end
  
    if (state == STATE_INIT) begin
      state <= STATE_PARAM;
    end
    else if (state == STATE_PARAM) begin
      if (param_done) begin
        state <= STATE_PIPELINE;
        //line_cycle <= 0;
        //x <= 0;
      end
    end
    else if (state == STATE_PIPELINE) begin
      if (line_cycle == LINE_CYCLE_VISIBLE) begin
        state <= STATE_WAITSYNC;
      end
    end
    else /*if (state == STATE_WAITSYNC)*/ begin
      if (line_cycle == 0) begin
        state <= STATE_PARAM;
      end
    end
  end
end

vpu_bg_parameter_load vpu_bg_parameter_load (
  clk,
  rst_n & (state == STATE_PARAM),
  sp_en,
  sp_addr,
  sp_dout,
  bg_data0_cache,
  bg_data1_cache,
  bg_affine0_cache,
  bg_affine1_cache,
  bg_affine2_cache,
  param_done
);

reg [31:0] color_debug;
assign pipeline_enable = (state == STATE_PIPELINE);
reg dummy;
vpu_bg_pipeline vpu_bg_pipeline (
  clk,
  rst_n,
  pipeline_enable,
  y,
  bg_data0_cache,
  bg_data1_cache,
  bg_affine0_cache,
  bg_affine1_cache,
  bg_affine2_cache,
  map_en,
  map_addr,
  map_dout,
  tile_en,
  tile_addr,
  tile_dout,
  pal_en,
  pal_addr,
  pal_dout,
  color,
  dummy
);

assign hsync = (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
assign vsync = y < SCREEN_H;

endmodule

/* -------------------------------- */
module vpu_bg_parameter_load (
  input  wire        clk,
  input  wire        rst_n,
  output wire        bg_en,
  output wire [ 9:0] bg_addr,
  input  wire [31:0] bg_dout,
  output reg  [31:0] bg_data0_cache[4],
  output reg  [31:0] bg_data1_cache[4],
  output reg  [31:0] bg_affine0_cache[4],
  output reg  [31:0] bg_affine1_cache[4],
  output reg  [31:0] bg_affine2_cache[4],
  output reg         done
);

localparam PARAM_SIZE = 5;
localparam SPRITE_RAM_BG_BASE = 128 * PARAM_SIZE;

reg [1:0] layer;
reg [2:0] offset;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    layer <= 0;
    offset <= 0;
  end
  else begin
    offset <= (offset < PARAM_SIZE-1) ? offset + 1 : 0;
    if (offset == PARAM_SIZE-1) begin
      layer <= (layer < 4-1) ? layer + 1 : 0;
    end
  end
end

assign done = (offset == PARAM_SIZE-1 && layer == 3);

always_comb begin
   case(offset)
   0: bg_data0_cache[layer] = bg_dout;
   1: bg_data1_cache[layer] = bg_dout;
   2: bg_affine0_cache[layer] = bg_dout;
   3: bg_affine1_cache[layer] = bg_dout;
   4: bg_affine2_cache[layer] = bg_dout;
   default: ;
   endcase
end

assign bg_addr = SPRITE_RAM_BG_BASE + (layer * PARAM_SIZE) + offset;
assign bg_en = !done;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline #(
    parameter SCREEN_W = 320,
    parameter SCREEN_H = 240,
    parameter SCREEN_HBLANK = 80,
    parameter SCREEN_VBLANK = 80
)(
  input  wire        clk,
  input  wire        rst_n,
  input  wire        pipeline_enable,
  input  wire [ 7:0] y,
  input  reg  [31:0] bg_data0_cache[4],
  input  reg  [31:0] bg_data1_cache[4],
  input  reg  [31:0] bg_affine0_cache[4],
  input  reg  [31:0] bg_affine1_cache[4],
  input  reg  [31:0] bg_affine2_cache[4],
  output wire        map_en,
  output wire [10:0] map_addr,
  input  wire [15:0] map_data,
  output wire        tile_en,
  output wire [14:0] tile_addr,
  input  wire [ 7:0] tile_data,
  output wire        pal_en,
  output wire [ 7:0] pal_addr,
  input  wire [31:0] pal_data,
  output wire [31:0] color,
  output wire        dummy
);

reg [1:0] layer;

reg [8:0] x;
wire [31:31] bg_enable;
wire [30:30] bg_afen;
wire [29:28] bg_layer;
wire [27:27] bg_hflip;
wire [26:26] bg_vflip;
wire [25:24] bg_tilesize;
wire [23:23] bg_mode;
wire [16: 8] bg_x;
wire [ 7: 0] bg_y;
wire [95: 0] bg_affine;

assign bg_enable   = bg_data0_cache[layer][31:31];
assign bg_afen     = bg_data0_cache[layer][30:30];
assign bg_layer    = bg_data0_cache[layer][29:28];
assign bg_hflip    = bg_data0_cache[layer][27:27];
assign bg_vflip    = bg_data0_cache[layer][26:26];
assign bg_tilesize = bg_data0_cache[layer][25:24];
assign bg_mode     = bg_data0_cache[layer][23:23];
assign bg_x        = bg_data0_cache[layer][16: 8];
assign bg_y        = bg_data0_cache[layer][ 7: 0];
assign bg_affine = {bg_affine0_cache[layer], bg_affine1_cache[layer], bg_affine2_cache[layer]};

always_ff @(posedge clk) begin
  if (~rst_n) begin
    layer <= 0;
    x <= 0;
  end
  else begin
    if (pipeline_enable) begin
      layer <= (layer < 4-1) ? layer + 1 : 0;
      if (layer == 4-1) begin
        x <= (x < SCREEN_W-1) ? x + 1 : 0;
      end
    end
    else begin
      layer <= 0;
      x <= 0;
    end
  end
end

reg [8:0] objx;
reg [7:0] objy;
reg [8:0] xbegin;
reg [8:0] xend;
reg [7:0] tw;
reg [7:0] th;
vpu_bg_pipeline0_affine_transform bg_pipe0(
  clk,
  rst_n,
  x,
  y,
  bg_tilesize,
  bg_x,
  bg_y,
  bg_afen,
  bg_affine,
  objx,
  objy,
  xbegin,
  xend,
  tw,
  th
);

reg [1:0] layer_p01;
reg [8:0] objx_p01;
reg [7:0] objy_p01;
reg [7:0] tw_p01;
reg [7:0] th_p01;
always_ff @(posedge clk) begin
  layer_p01 <= layer;
  objx_p01 <= objx;
  objy_p01 <= objy;
  tw_p01 <= tw;
  th_p01 <= th;
end

reg [8:0] tx0;
reg [8:0] ty0;
wire [15:0] tile_idx;
vpu_bg_pipeline1_map_load bg_pipe1 (
  clk,
  rst_n,
  objx_p01,
  objy_p01,
  tw_p01,
  th_p01,
  tx0,
  ty0,
  map_en,
  map_addr,
  map_data,
  tile_idx
);

reg [1:0] layer_p12;
reg [8:0] objx_p12;
reg [7:0] objy_p12;
reg [15:0] tile_idx_p12;
always_ff @(posedge clk) begin
  layer_p12 <= layer_p01;
  objx_p12 <= objx_p01;
  objy_p12 <= objy_p01;
  tile_idx_p12 <= tile_idx;
end

reg [7:0] pal_idx;
vpu_bg_pipeline2_tile_load bg_pipe2 (
  clk,
  rst_n,
  objx_p12,
  objy_p12,
  tile_idx_p12,
  tile_en,
  tile_addr,
  tile_data,
  pal_idx
);

reg [1:0] layer_p23;
reg [7:0] pal_idx_p23;
always_ff @(posedge clk) begin
  layer_p23 <= layer_p12;
  pal_idx_p23 <= pal_idx;
end

wire [31:0] color_n[4];
vpu_bg_pipeline3_palette_load  bg_pipe3 (
  clk,
  rst_n,
  layer_p23,
  pal_idx_p23,
  pal_en,
  pal_addr,
  pal_data,
  color_n
);

reg [1:0] layer_p34;
reg [31:0] color_n_p34[4];
always_ff @(posedge clk) begin
  layer_p34 <= layer_p23;
  color_n_p34 <= color_n;
end

reg [31:0] color_out;
reg done;
vpu_bg_pipeline4_color_merge bg_pipe4 (
  clk,
  rst_n,
  layer_p34,
  color_n_p34,
  color_out,
  done
);

assign color = color_out;
assign dummy = 1;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline0_affine_transform (
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] xin,
  input  wire [ 7:0] yin,
  input  wire [ 1:0] bg_tilesize,
  input  wire [ 8:0] bg_x,
  input  wire [ 7:0] bg_y,
  input  wire        bg_afen,
  input  wire [95:0] bg_affine,
  output reg  [ 8:0] objx,
  output reg  [ 7:0] objy,
  output reg  [ 8:0] xbegin,
  output reg  [ 8:0] xend,
  output reg  [ 7:0] tw,
  output reg  [ 7:0] th
);

function logic [7:0] get_tw(input [2:0] tilesize);
   case(tilesize)
     2'b00 : get_tw = 16'd8;
     2'b01 : get_tw = 16'd16;
     2'b10 : get_tw = 16'd32;
     2'b11 : get_tw = 16'd64;
   endcase
endfunction
function logic [7:0] get_th(input [2:0] tilesize);
   case(tilesize)
     2'b00 : get_th = 16'd8;
     2'b01 : get_th = 16'd16;
     2'b10 : get_th = 16'd32;
     2'b11 : get_th = 16'd64;
   endcase
endfunction

always_comb begin
  objx = xin - bg_x;
  objy = yin - bg_y;
  tw = get_tw(bg_tilesize);
  th = get_th(bg_tilesize);
  // [TODO] affine transform
  if (bg_afen) begin
  end
end

endmodule

/* -------------------------------- */
module vpu_bg_pipeline1_map_load (
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] objx,
  input  wire [ 7:0] objy,
  input  wire [ 7:0] tw,
  input  wire [ 7:0] th,
  output reg  [ 8:0] tx0,
  output reg  [ 8:0] ty0,
  output wire        map_en,
  output reg  [10:0] map_addr,
  input  reg  [15:0] map_data,
  output reg  [15:0] tile_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;

reg [1:0] map_bank = 0;
shortint offset0 = 0;
shortint offset1x = 0;
shortint offset1y = 0;

always_comb begin
  tx0 = objx & (HW_TILEMAP_W-1);
  ty0 = objy & (HW_TILEMAP_H-1);
  offset0 = (tx0 / tw) + (ty0 / th) * (HW_TILEMAP_W / tw);
  //offset1x = (tx0 & (tw-1)) / HW_TILE_W;
  //offset1y = (ty0 & (th-1)) / HW_TILE_H * (tw / HW_TILE_W);
  offset1x = (tx0 % tw) / HW_TILE_W;
  offset1y = (ty0 % th) / HW_TILE_H * (tw / HW_TILE_W);
  tile_idx = map_data * (tw * th / HW_TILE_H / HW_TILE_W) + offset1x + offset1y;
end

assign map_en = 1;
assign map_addr = offset0;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline2_tile_load (
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] objx,
  input  wire [ 7:0] objy,
  input  wire [15:0] tile_idx,
  output wire        tile_en,
  output reg  [14:0] tile_addr,
  input  reg  [ 7:0] tile_data,
  output reg  [ 7:0] pal_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;

shortint tilex;
shortint tiley;

always_comb begin
  //tilex = (objx & (HW_TILEMAP_W-1)) & (HW_TILE_W-1);
  //tiley = (objy & (HW_TILEMAP_H-1)) & (HW_TILE_H-1);
  tilex = (objx % HW_TILEMAP_W) % HW_TILE_W;
  tiley = (objy % HW_TILEMAP_H) % HW_TILE_H;
  tile_addr = (tile_idx * HW_TILE_W * HW_TILE_H) + (tiley * HW_TILE_W) + tilex;
end

assign tile_en = 1;
assign pal_idx = tile_data;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline3_palette_load (
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 1:0] layer,
  input  wire [ 7:0] pal_idx,
  output wire        pal_en,
  output reg  [ 7:0] pal_addr,
  input  reg  [31:0] pal_data,
  output reg  [31:0] color_n[4]
);

//always_comb begin
always_ff @(posedge clk) begin
  pal_addr <= pal_idx;
  color_n[layer] <= pal_data;
end

assign pal_en = 1;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline4_color_merge (
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 1:0] layer,
  input  wire [31:0] color_n[4],
  output reg  [31:0] color_out,
  output reg         done
);

//always_comb begin
always_ff @(posedge clk) begin
  if (layer == 3) begin
    color_out <= color_n[0];
    done <= 1;
  end
  else begin
    done <= 0;
  end
end

endmodule