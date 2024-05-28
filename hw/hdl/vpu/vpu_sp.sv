module vpu_sp
  import gameconsole_pkg::*;
(
  input  wire                   clk,
  input  wire                   rst_n,

  input  wire [10:0]            line_cycle,
  input  wire [8:0]             y,

  output wire                   param_en,
  output wire [SP_ADDR_W-1:0]   param_addr,
  input  wire [SP_DATA_W-1:0]   param_dout,

  output wire                   tile_en,
  output wire [TILE_ADDR_W-1:0] tile_addr,
  input  wire [TILE_DATA_W-1:0] tile_dout,

  output wire                   pal_en,
  output wire [PAL_ADDR_W-1:0]  pal_addr,
  input  wire [PAL_DATA_W-1:0]  pal_dout,

  output wire [31:0]            sp_linebuffer[320]
);

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;
localparam LINE_CYCLE_PARAM = 20;
localparam LINE_CYCLE_SPPARAM = 0;
localparam LINE_CYCLE_VISIBLE = LINE_CYCLE_PARAM + SCREEN_W * 4;
localparam LINE_CYCLE_MAX = LINE_CYCLE_PARAM + HMAX * 4;

reg [31:0] linebuffer[320];

reg [31:0] sp_data0_cache[4];
reg [31:0] sp_data1_cache[4];
reg [31:0] sp_affine0_cache[4];
reg [31:0] sp_affine1_cache[4];
reg [31:0] sp_affine2_cache[4];

wire [8:0] x = 0;
reg [10:0] sp_cycle = 0;
reg [1:0] sp_layer = 0;

reg param_done;

reg pipeline_enable;

enum {
  STATE_INIT,
  STATE_PARAM,
  STATE_SPPARAM,
  STATE_PIPELINE,
  STATE_WAITSYNC
} state;

assign x = line_cycle / 4;

always_comb begin
  if (line_cycle < LINE_CYCLE_PARAM) begin
    state = STATE_PARAM;
  end
  else if (line_cycle < LINE_CYCLE_VISIBLE) begin
    if (!param_done) begin
      state = STATE_SPPARAM;
    end else begin
      state = STATE_PIPELINE;
    end
  end
  else begin
    state = STATE_WAITSYNC;
  end
end

vpu_sp_parameter_load vpu_sp_parameter_load (
  clk,
  rst_n,
  // parameter_enable,
  param_en,
  param_addr,
  param_dout,
  sp_data0_cache,
  sp_data1_cache,
  sp_affine0_cache,
  sp_affine1_cache,
  sp_affine2_cache,
  param_done
);

reg [31:0] color_debug;
assign pipeline_enable = (state == STATE_PIPELINE);
reg dummy;
vpu_sp_pipeline vpu_sp_pipeline (
  clk,
  rst_n,
  pipeline_enable,
  y,
  sp_data0_cache,
  sp_data1_cache,
  sp_affine0_cache,
  sp_affine1_cache,
  sp_affine2_cache,
  tile_en,
  tile_addr,
  tile_dout,
  pal_en,
  pal_addr,
  pal_dout,
  color_debug,
  dummy
);

assign sp_linebuffer = linebuffer;
// assign hsync = (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
// assign vsync = y < SCREEN_H;

endmodule

/* -------------------------------- */
module vpu_sp_parameter_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  output wire        param_en,
  output wire [ 9:0] param_addr,
  input  wire [31:0] param_dout,
  output reg  [31:0] sp_data0_cache[4],
  output reg  [31:0] sp_data1_cache[4],
  output reg  [31:0] sp_affine0_cache[4],
  output reg  [31:0] sp_affine1_cache[4],
  output reg  [31:0] sp_affine2_cache[4],
  output reg         done
);

localparam PARAM_SIZE = 5;

reg [1:0] layer;
reg [2:0] offset;
reg [2:0] offset_prev;

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
  offset_prev <= offset;
end

assign done = (offset == PARAM_SIZE-1 && layer == 3);

always_comb begin
   case(offset_prev)
   0: sp_data0_cache  [layer] = param_dout;
   1: sp_data1_cache  [layer] = param_dout;
   2: sp_affine0_cache[layer] = param_dout;
   3: sp_affine1_cache[layer] = param_dout;
   4: sp_affine2_cache[layer] = param_dout;
   default: ;
   endcase
end

assign param_addr = (layer * PARAM_SIZE) + offset;
assign param_en = !done;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire        pipeline_enable,
  input  wire [ 7:0] y,
  input  reg  [31:0] sp_data0_cache[4],
  input  reg  [31:0] sp_data1_cache[4],
  input  reg  [31:0] sp_affine0_cache[4],
  input  reg  [31:0] sp_affine1_cache[4],
  input  reg  [31:0] sp_affine2_cache[4],
  output wire                 tile_en,
  output wire [TILE_ADDR_W-1:0] tile_addr,
  input  wire [TILE_DATA_W-1:0] tile_data,
  output wire                 pal_en,
  output wire [PAL_ADDR_W-1:0]  pal_addr,
  input  wire [PAL_DATA_W-1:0]  pal_data,
  output wire [31:0] color,
  output wire        dummy
);

reg [1:0] layer;

reg [8:0] x;
wire [ 0: 0]/*[31:31]*/ sp_enable;
wire [ 0: 0]/*[30:30]*/ sp_afen;
wire [ 1: 0]/*[29:28]*/ sp_layer;
wire [ 0: 0]/*[27:27]*/ sp_hflip;
wire [ 0: 0]/*[26:26]*/ sp_vflip;
wire [ 1: 0]/*[25:24]*/ sp_tilesize;
// wire [ 0: 0]/*[23:17]*/ sp__reserved;
wire [ 8: 0]/*[16: 8]*/ sp_x;
wire [ 7: 0]/*[ 7: 0]*/ sp_y;

wire [ 3: 0]/*[31:28]*/ sp_tilebank;
wire [11: 0]/*[27:16]*/ sp_tileidx;
//wire [ 3: 0]/*[15:12]*/ sp__palreserved0;
wire [ 3: 0]/*[11: 8]*/ sp_pal_transparency;
//wire [ 0: 0]/*[ 7: 7]*/ sp__palreserved1;
wire [ 0: 0]/*[ 6: 6]*/ sp_pal_mode;
wire [ 1: 0]/*[ 5: 4]*/ sp_pal_bank;
wire [ 3: 0]/*[ 3: 0]*/ sp_pal_no;

wire [95: 0] sp_affine;

assign sp_enable   = sp_data0_cache[layer][31:31];
assign sp_afen     = sp_data0_cache[layer][30:30];
assign sp_layer    = sp_data0_cache[layer][29:28];
assign sp_hflip    = sp_data0_cache[layer][27:27];
assign sp_vflip    = sp_data0_cache[layer][26:26];
assign sp_tilesize = sp_data0_cache[layer][25:24];
// assign sp__reserved = sp_data0_cache[layer][23:17];
assign sp_x        = sp_data0_cache[layer][16: 8];
assign sp_y        = sp_data0_cache[layer][ 7: 0];

assign sp_tile_bank        = sp_data1_cache[layer][31:28];
assign sp_tile_idx    = sp_data1_cache[layer][27:16];
//assign sp__palreserved0    = sp_data1_cache[layer][15:12];
assign sp_pal_transparency = sp_data1_cache[layer][11: 8];
//assign sp__palreserved1    = sp_data1_cache[layer][ 7: 7];
assign sp_pal_mode         = sp_data1_cache[layer][ 6: 6];
assign sp_pal_bank         = sp_data1_cache[layer][ 5: 4];
assign sp_pal_no           = sp_data1_cache[layer][ 3: 0];

assign sp_affine = {sp_affine0_cache[layer], sp_affine1_cache[layer], sp_affine2_cache[layer]};

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
vpu_sp_pipeline0_affine_transform sp_pipe0(
  clk,
  rst_n,
  x,
  y,
  sp_tilesize,
  sp_x,
  sp_y,
  sp_afen,
  sp_affine,
  objx,
  objy,
  xbegin,
  xend,
  tw,
  th
);

reg [1:0] layer_p02;
reg [8:0] objx_p02;
reg [7:0] objy_p02;
reg [7:0] tw_p02;
reg [7:0] th_p02;
reg [15:0] tile_idx_p02;
reg [0:0] pal_mode_p02;
reg [1:0] pal_bank_p02;
reg [3:0] pal_no_p02;
always_ff @(posedge clk) begin
  layer_p02 <= layer;
  objx_p02 <= objx;
  objy_p02 <= objy;
  tw_p02 <= tw;
  th_p02 <= th;
  tile_idx_p02 <= sp_tile_idx;
  pal_mode_p02 <= sp_pal_mode;
  pal_bank_p02 <= sp_pal_bank;
  pal_no_p02 <= sp_pal_no;
end

reg [7:0] pal_idx;
vpu_sp_pipeline2_tile_load sp_pipe2 (
  clk,
  rst_n,
  objx_p02,
  objy_p02,
  tw_p02,
  tile_idx_p02,
  tile_en,
  tile_addr,
  tile_data,
  pal_idx
);

reg [1:0] layer_p23;
reg [7:0] pal_idx_p23;
reg [0:0] pal_mode_p23;
reg [1:0] pal_bank_p23;
reg [3:0] pal_no_p23;
always_ff @(posedge clk) begin
  layer_p23 <= layer_p02;
  pal_mode_p23 <= pal_mode_p02;
  pal_bank_p23 <= pal_bank_p02;
  pal_no_p23 <= pal_no_p02;
  pal_idx_p23 <= pal_idx;
end

wire [31:0] color_n[4];
vpu_sp_pipeline3_palette_load  sp_pipe3 (
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

reg [1:0] layer_p34;
reg [31:0] color_n_p34[4];
always_ff @(posedge clk) begin
  layer_p34 <= layer_p23;
  color_n_p34 <= color_n;
end

reg [31:0] color_out;
reg done;
vpu_sp_pipeline4_color_merge sp_pipe4 (
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
module vpu_sp_pipeline0_affine_transform
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] xin,
  input  wire [ 7:0] yin,
  input  wire [ 1:0] sp_tilesize,
  input  wire [ 8:0] sp_x,
  input  wire [ 7:0] sp_y,
  input  wire        sp_afen,
  input  wire [95:0] sp_affine,
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
  objx = xin - sp_x;
  objy = yin - sp_y;
  tw = get_tw(sp_tilesize);
  th = get_th(sp_tilesize);
  // [TODO] affine transform
  if (sp_afen) begin
  end
end

endmodule

/* -------------------------------- */
module vpu_sp_pipeline2_tile_load
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 8:0] objx,
  input  wire [ 7:0] objy,
  input  wire [ 7:0] tw,
  input  wire [TILE_INDX_W-1:0] sp_tile_idx,
  output wire        tile_en,
  output reg  [TILE_ADDR_W-1:0] tile_addr,
  input  reg  [TILE_DATA_W-1:0] tile_data,
  output reg  [PAL_INDX_W-1:0] pal_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;


shortint offset = 0;
shortint tilex;
shortint tiley;
reg  [ 8:0] tx0;
reg  [ 8:0] ty0;
reg  [15:0] tile_idx;

always_comb begin
  tx0 = objx & (HW_TILEMAP_W-1);
  ty0 = objy & (HW_TILEMAP_H-1);
  offset = (tx0 / HW_TILE_W) + (ty0 / HW_TILE_H) * (tw / HW_TILE_W);
  tile_idx = sp_tile_idx + offset;

  tilex = (objx & (HW_TILEMAP_W-1)) & (HW_TILE_W-1);
  tiley = (objy & (HW_TILEMAP_H-1)) & (HW_TILE_H-1);
  tile_addr = (tile_idx * HW_TILE_W * HW_TILE_H) + (tiley * HW_TILE_W) + tilex;
end

assign tile_en = 1;
assign pal_idx = tile_data;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline3_palette_load
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
module vpu_sp_pipeline4_color_merge
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,
  input  wire [ 1:0] layer,
  input  wire [31:0] color_n[4],
  output reg  [31:0] color_out,
  output reg         done
);

function logic [31:0] color_merge(logic [31:0] cols[4]);
  logic [31:0] dst = 0;
  logic [31:0] src;
  logic [ 7:0] src_a;
  for (int i=0; i<4; i++) begin
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

//always_comb begin
always_ff @(posedge clk) begin
  if (layer == 3) begin
    color_out <= color_merge(color_n);
    done <= 1;
  end
  else begin
    done <= 0;
  end
end

endmodule
