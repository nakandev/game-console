module vpu_bg
  import gameconsole_pkg::*;
(
  input  wire                   clk,
  input  wire                   rst_n,

  input  wire [10:0]            line_cycle,
  input  wire [8:0]             y,

  output wire                   param_en,
  output wire [BG_ADDR_W-1:0]   param_addr,
  input  wire [BG_DATA_W-1:0]   param_dout,

  output wire                   map_en,
  output wire [MAP_ADDR_W-1:0]  map_addr,
  input  wire [MAP_DATA_W-1:0]  map_dout,

  output wire                   tile_en,
  output wire [TILE_ADDR_W-1:0] tile_addr,
  input  wire [TILE_DATA_W-1:0] tile_dout,

  output wire                   pal_en,
  output wire [PAL_ADDR_W-1:0]  pal_addr,
  input  wire [PAL_DATA_W-1:0]  pal_dout,

  output wire [LINEBUFF_BANK_W-1:0] line_wea,
  output wire [LINEBUFF_ADDR_W-1:0] line_addra,
  output reg  [LINEBUFF_DATA_W-1:0] line_dina,
  input  wire [LINEBUFF_DATA_W-1:0] line_douta,

  output wire                   bg_param,
  output wire                   dot_clk,
  output reg  [31:0]            color,
  output wire                   hsync,
  output wire                   vsync
);

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;
localparam LINE_CYCLE_PARAM = 20;
localparam LINE_CYCLE_VISIBLE = LINE_CYCLE_PARAM + SCREEN_W * 4;
localparam LINE_CYCLE_MAX = LINE_CYCLE_PARAM + HMAX * 4;

reg [31:0] bg_data0_cache[4];
reg [31:0] bg_data1_cache[4];
reg [31:0] bg_affine0_cache[4];
reg [31:0] bg_affine1_cache[4];
reg [31:0] bg_affine2_cache[4];

wire [8:0] x = 0;

reg pipeline_enable;

enum {
  STATE_INIT,
  STATE_PARAM,
  STATE_PIPELINE,
  STATE_WAITSYNC
} state;

assign x = line_cycle / 4;

assign state = line_cycle < LINE_CYCLE_PARAM   ? STATE_PARAM :
               line_cycle < LINE_CYCLE_VISIBLE ? STATE_PIPELINE :
               STATE_WAITSYNC;

assign parameter_enable = (state == STATE_PARAM);
assign bg_param = parameter_enable;
vpu_bg_parameter_load vpu_bg_parameter_load (
  clk,
  rst_n,
  parameter_enable,
  param_en,
  param_addr,
  param_dout,
  bg_data0_cache,
  bg_data1_cache,
  bg_affine0_cache,
  bg_affine1_cache,
  bg_affine2_cache
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
  line_wea,
  line_addra,
  line_dina,
  line_douta,
  color,
  dummy
);

assign hsync = (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
assign vsync = y < SCREEN_H;

endmodule

/* -------------------------------- */
module vpu_bg_parameter_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire        parameter_enable,
  output wire        bg_en,
  output wire [ 9:0] bg_addr,
  input  wire [31:0] bg_dout,
  output reg  [31:0] bg_data0_cache[4],
  output reg  [31:0] bg_data1_cache[4],
  output reg  [31:0] bg_affine0_cache[4],
  output reg  [31:0] bg_affine1_cache[4],
  output reg  [31:0] bg_affine2_cache[4]
);

localparam PARAM_SIZE = 5;
localparam SPRITE_RAM_BG_BASE = 128 * PARAM_SIZE;

reg [1:0] layer;
reg [2:0] offset;
reg [2:0] offset_prev;

always_ff @(posedge clk) begin
  if (~(rst_n & parameter_enable)) begin
    layer <= 0;
    offset <= 0;
  end
  else begin
    offset <= (offset < PARAM_SIZE-1) ? offset + 1 : 0;
    if (offset == PARAM_SIZE-1) begin
      layer <= (layer < 4-1) ? layer + 1 : 0;
    end
  end
  offset_prev <= offset;
end

always_comb begin
  if (parameter_enable) begin
    case(offset_prev)
    0: bg_data0_cache[layer] = bg_dout;
    1: bg_data1_cache[layer] = bg_dout;
    2: bg_affine0_cache[layer] = bg_dout;
    3: bg_affine1_cache[layer] = bg_dout;
    4: bg_affine2_cache[layer] = bg_dout;
    default: ;
    endcase
  end
end

assign bg_addr = SPRITE_RAM_BG_BASE + (layer * PARAM_SIZE) + offset;
assign bg_en = parameter_enable;
// assign bg_en = 1;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire        pipeline_enable,
  input  wire [ 7:0] y,
  input  reg  [31:0] bg_data0_cache[4],
  input  reg  [31:0] bg_data1_cache[4],
  input  reg  [31:0] bg_affine0_cache[4],
  input  reg  [31:0] bg_affine1_cache[4],
  input  reg  [31:0] bg_affine2_cache[4],
  output wire                 map_en,
  output wire [MAP_ADDR_W-1:0]  map_addr,
  input  wire [MAP_DATA_W-1:0]  map_data,
  output wire                 tile_en,
  output wire [TILE_ADDR_W-1:0] tile_addr,
  input  wire [TILE_DATA_W-1:0] tile_data,
  output wire                 pal_en,
  output wire [PAL_ADDR_W-1:0]  pal_addr,
  input  wire [PAL_DATA_W-1:0]  pal_data,
  output wire [LINEBUFF_BANK_W-1:0] line_wea,
  output wire [LINEBUFF_ADDR_W-1:0] line_addra,
  output reg  [LINEBUFF_DATA_W-1:0] line_dina,
  input  wire [LINEBUFF_DATA_W-1:0] line_douta,
  output wire [31:0] color,
  output wire        dummy
);

reg [1:0] layer;

reg [8:0] x;
wire [ 0: 0]/*[31:31]*/ bg_enable;
wire [ 0: 0]/*[30:30]*/ bg_afen;
wire [ 1: 0]/*[29:28]*/ bg_layer;
wire [ 0: 0]/*[27:27]*/ bg_hflip;
wire [ 0: 0]/*[26:26]*/ bg_vflip;
wire [ 1: 0]/*[25:24]*/ bg_tilesize;
wire [ 0: 0]/*[23:23]*/ bg_mode;
// wire [ 5: 0]/*[22:17]*/ bg__reserved;
wire [ 8: 0]/*[16: 8]*/ bg_x;
wire [ 7: 0]/*[ 7: 0]*/ bg_y;

wire [ 3: 0]/*[31:28]*/ bg_bank;
wire [ 3: 0]/*[27:24]*/ bg_map_bank;
//wire [ 7: 0]/*[23:16]*/ bg__tilereserved;
//wire [ 3: 0]/*[15:12]*/ bg__palreserved0;
wire [ 3: 0]/*[11: 8]*/ bg_pal_transparency;
//wire [ 0: 0]/*[ 7: 7]*/ bg__palreserved1;
wire [ 0: 0]/*[ 6: 6]*/ bg_pal_mode;
wire [ 1: 0]/*[ 5: 4]*/ bg_pal_bank;
wire [ 3: 0]/*[ 3: 0]*/ bg_pal_no;

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

assign bg_bank             = bg_data1_cache[layer][31:28];
assign bg_map_bank         = bg_data1_cache[layer][27:24];
//assign bg__tilereserved    = bg_data1_cache[layer][23:16];
//assign bg__palreserved0    = bg_data1_cache[layer][15:12];
assign bg_pal_transparency = bg_data1_cache[layer][11: 8];
//assign bg__palreserved1    = bg_data1_cache[layer][ 7: 7];
assign bg_pal_mode         = bg_data1_cache[layer][ 6: 6];
assign bg_pal_bank         = bg_data1_cache[layer][ 5: 4];
assign bg_pal_no           = bg_data1_cache[layer][ 3: 0];

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

reg [8:0] x_p01;
reg [1:0] layer_p01;
reg [8:0] objx_p01;
reg [7:0] objy_p01;
reg [7:0] tw_p01;
reg [7:0] th_p01;
reg [3:0] map_bank_p01;
reg [0:0] pal_mode_p01;
reg [1:0] pal_bank_p01;
reg [3:0] pal_no_p01;
always_ff @(posedge clk) begin
  x_p01 <= x;
  layer_p01 <= layer;
  objx_p01 <= objx;
  objy_p01 <= objy;
  tw_p01 <= tw;
  th_p01 <= th;
  map_bank_p01 <= bg_map_bank;
  pal_mode_p01 <= bg_pal_mode;
  pal_bank_p01 <= bg_pal_bank;
  pal_no_p01 <= bg_pal_no;
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
  map_bank_p01,
  map_en,
  map_addr,
  map_data,
  tile_idx
);

reg [8:0] x_p12;
reg [1:0] layer_p12;
reg [8:0] objx_p12;
reg [7:0] objy_p12;
reg [15:0] tile_idx_p12;
reg [0:0] pal_mode_p12;
reg [1:0] pal_bank_p12;
reg [3:0] pal_no_p12;
always_ff @(posedge clk) begin
  x_p12 <= x_p01;
  layer_p12 <= layer_p01;
  objx_p12 <= objx_p01;
  objy_p12 <= objy_p01;
  tile_idx_p12 <= tile_idx;
  pal_mode_p12 <= pal_mode_p01;
  pal_bank_p12 <= pal_bank_p01;
  pal_no_p12 <= pal_no_p01;
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

reg [8:0] x_p23;
reg [1:0] layer_p23;
reg [7:0] pal_idx_p23;
reg [0:0] pal_mode_p23;
reg [1:0] pal_bank_p23;
reg [3:0] pal_no_p23;
always_ff @(posedge clk) begin
  x_p23 <= x_p12;
  layer_p23 <= layer_p12;
  pal_mode_p23 <= pal_mode_p12;
  pal_bank_p23 <= pal_bank_p12;
  pal_no_p23 <= pal_no_p12;
  pal_idx_p23 <= pal_idx;
end

wire [31:0] color_n[4];
vpu_bg_pipeline3_palette_load  bg_pipe3 (
  clk,
  rst_n,
  layer_p23,
  pal_mode_p23,
  pal_bank_p23,
  pal_no_p23,
  pal_idx_p23,
  pal_en,
  pal_addr,
  pal_data,
  color_n
);

reg [8:0] x_p34;
reg [1:0] layer_p34;
reg [31:0] color_n_p34[4];
always_ff @(posedge clk) begin
  x_p34 <= x_p23;
  layer_p34 <= layer_p23;
  color_n_p34 <= color_n;
end

reg [31:0] color_out;
reg done;
vpu_bg_pipeline4_color_merge bg_pipe4 (
  clk,
  rst_n,
  x_p34,
  layer_p34,
  color_n_p34,
  line_wea,
  line_addra,
  line_dina,
  line_douta,
  color_out,
  done
);

assign color = color_out;
assign dummy = 1;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline0_affine_transform
  import gameconsole_pkg::*;
(
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
module vpu_bg_pipeline1_map_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] objx,
  input  wire [ 7:0] objy,
  input  wire [ 7:0] tw,
  input  wire [ 7:0] th,
  output reg  [ 8:0] tx0,
  output reg  [ 8:0] ty0,
  input  wire [ 3:0] map_bank,
  output wire        map_en,
  output reg  [MAP_ADDR_W-1:0] map_addr,
  input  reg  [MAP_DATA_W-1:0] map_data,
  output reg  [15:0] tile_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;

shortint offset0 = 0;
shortint offset1x = 0;
shortint offset1y = 0;

always_comb begin
  tx0 = objx & (HW_TILEMAP_W-1);
  ty0 = objy & (HW_TILEMAP_H-1);
  offset0 = (tx0 / tw) + (ty0 / th) * (HW_TILEMAP_W / tw);
  offset1x = (tx0 & (tw-1)) / HW_TILE_W;
  offset1y = (ty0 & (th-1)) / HW_TILE_H * (tw / HW_TILE_W);
  tile_idx = map_data * (tw * th / HW_TILE_H / HW_TILE_W) + offset1x + offset1y;
end

assign map_en = 1;
assign map_addr = {map_bank, offset0[10:0]};

endmodule

/* -------------------------------- */
module vpu_bg_pipeline2_tile_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] objx,
  input  wire [ 7:0] objy,
  input  wire [TILE_INDX_W-1:0] tile_idx,
  output wire        tile_en,
  output reg  [TILE_ADDR_W-1:0] tile_addr,
  input  reg  [TILE_DATA_W-1:0] tile_data,
  output reg  [PAL_INDX_W-1:0] pal_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;

shortint tilex;
shortint tiley;

always_comb begin
  tilex = (objx & (HW_TILEMAP_W-1)) & (HW_TILE_W-1);
  tiley = (objy & (HW_TILEMAP_H-1)) & (HW_TILE_H-1);
  tile_addr = (tile_idx * HW_TILE_W * HW_TILE_H) + (tiley * HW_TILE_W) + tilex;
end

assign tile_en = 1;
assign pal_idx = tile_data;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline3_palette_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 1:0] layer,
  input  wire        pal_mode,
  input  wire [PAL_BANK_W-1:0] pal_bank,
  input  wire [ 3:0] pal_no,
  input  wire [PAL_INDX_W-1:0] pal_idx,
  output wire        pal_en,
  output reg  [PAL_ADDR_W-1:0] pal_addr,
  input  reg  [PAL_DATA_W-1:0] pal_data,
  output reg  [31:0] color_n[4]
);

//always_comb begin
always_ff @(posedge clk) begin
  if (pal_mode == 0) begin
    pal_addr <= {pal_bank, pal_idx};
  end
  else begin
    pal_addr <= {pal_bank, pal_idx & 8'h0F + pal_no * 16};
  end
  color_n[layer] <= pal_data;
end

assign pal_en = 1;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline4_color_merge
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] x,
  input  wire [ 1:0] layer,
  input  wire [31:0] color_n[4],
  output wire [LINEBUFF_BANK_W-1:0] line_wea,
  output wire [LINEBUFF_ADDR_W-1:0] line_addra,
  output reg  [LINEBUFF_DATA_W-1:0] line_dina,
  input  wire [LINEBUFF_DATA_W-1:0] line_douta,
  output reg  [31:0] color_out,
  output reg         done
);

function logic [31:0] color_merge(logic [31:0] cols[5]);
  logic [31:0] dst = 0;
  logic [31:0] src;
  logic [ 7:0] src_a;
  for (int i=0; i<5; i++) begin
    src = cols[i];
    src_a = src >> 24;
    if (src_a == 0) begin
      dst = dst;
    end
    else if (src_a == 255) begin
      dst = src;
    end
    else begin
      dst[ 7: 0] = ((dst[ 7: 0] * (255 - src_a) + src[ 7: 0] * src_a) / 256);
      dst[15: 8] = ((dst[15: 8] * (255 - src_a) + src[15: 8] * src_a) / 256);
      dst[23:16] = ((dst[23:16] * (255 - src_a) + src[23:16] * src_a) / 256);
      dst[31:24] = (dst[31:24] + src[31:24]) / 2;
     end
  end
  return dst;
endfunction

// assign line_wea = (layer == 3);
assign line_wea = 0;
assign line_addra = x;
assign line_dina = 0;

reg [31:0] colors[5];
assign colors[0] = color_n[0];
assign colors[1] = color_n[1];
assign colors[2] = color_n[2];
assign colors[3] = color_n[3];
// assign colors[4] = line_douta;
assign colors[4] = {line_douta, 16'h0, line_douta};

//always_comb begin
always_ff @(posedge clk) begin
  if (layer == 3) begin
    color_out <= color_merge(colors);
    // color_out <= colors[4];
    done <= 1;
  end
  else begin
    done <= 0;
  end
end

endmodule
