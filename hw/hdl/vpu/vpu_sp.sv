typedef enum {
  STATE_INIT,
  STATE_PARAM,
  STATE_SPPARAM,
  STATE_SPPIPELINE,
  STATE_SPIDLE,
  STATE_WAITSYNC
} vpu_sp_state_t;

function logic [7:0] get_tw(input [1:0] tilesize);
   case(tilesize)
     2'b00 : get_tw = 8'd8;
     2'b01 : get_tw = 8'd16;
     2'b10 : get_tw = 8'd32;
     2'b11 : get_tw = 8'd64;
   endcase
endfunction
function logic [7:0] get_th(input [1:0] tilesize);
   case(tilesize)
     2'b00 : get_th = 8'd8;
     2'b01 : get_th = 8'd16;
     2'b10 : get_th = 8'd32;
     2'b11 : get_th = 8'd64;
   endcase
endfunction

module vpu_sp
  import gameconsole_pkg::*;
(
  input  logic                   clk,
  input  logic                   rst_n,

  input  logic [10:0]            line_cycle,
  input  logic [8:0]             y,

  output logic                   param_en,
  output logic [PRM_ADDR_W-1:0]  param_addr,
  input  logic [PRM_DATA_W-1:0]  param_dout,

  output logic                   tile_en,
  output logic [TILE_ADDR_W-1:0] tile_addr,
  input  logic [TILE_DATA_W-1:0] tile_dout,

  output logic                   pal_en,
  output logic [PAL_ADDR_W-1:0]  pal_addr,
  input  logic [PAL_DATA_W-1:0]  pal_dout,

  output logic                       line_init,
  output logic [LINEBUFF_BANK_W-1:0] line_web,
  output logic [LINEBUFF_BANK_W-1:0] line_bankb,
  output logic [LINEBUFF_ADDR_W-1:0] line_addrb,
  output logic  [LINEBUFF_DATA_W-1:0] line_dinb,
  input  logic [LINEBUFF_DATA_W-1:0] line_doutb
);

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;
localparam LINE_CYCLE_PARAM = 20;
localparam LINE_CYCLE_SPPARAM = 5;
localparam LINE_CYCLE_VISIBLE = LINE_CYCLE_PARAM + SCREEN_W * 4;
localparam LINE_CYCLE_MAX = LINE_CYCLE_PARAM + HMAX * 4;

logic [31:0] sp_data0_cache;
logic [31:0] sp_data1_cache;
logic [31:0] sp_affine0_cache;
logic [31:0] sp_affine1_cache;
logic [31:0] sp_affine2_cache;

logic [8:0] x;
logic [10:0] sp_cycle;
logic [6:0]  sp_idx;

logic parameter_done;
logic pipeline_done;
vpu_sp_state_t state;

assign x = line_cycle / 4;

logic parameter_enable;
logic pipeline_enable;
assign parameter_enable = (state == STATE_SPPARAM);
assign pipeline_enable = (state == STATE_SPPIPELINE);
assign line_init = (state <= STATE_PARAM);
// always_comb begin
always_ff @(posedge clk) begin
  if (y >= SCREEN_H) begin
    state <= STATE_WAITSYNC;
    sp_cycle <= 0;
    sp_idx <= 0;
  end else
  if (line_cycle < LINE_CYCLE_PARAM) begin
    state <= STATE_PARAM;
    sp_cycle <= 0;
    sp_idx <= 0;
  end
  else if (line_cycle < LINE_CYCLE_VISIBLE) begin
    if (state == STATE_PARAM) begin
      state <= STATE_SPPARAM;
      sp_cycle <= 0;
      sp_idx <= sp_idx + 0;
    end
    else if (state == STATE_SPPARAM && sp_idx != 127 && parameter_done && sp_cycle == 1) begin
      state <= STATE_SPPARAM;
      sp_cycle <= 0;
      sp_idx <= sp_idx + 1;
    end
    else if (state == STATE_SPPARAM && sp_idx != 127 && parameter_done && sp_cycle > 1) begin
      state <= STATE_SPPIPELINE;
      sp_cycle <= sp_cycle + 1;
    end
    else if (state == STATE_SPPARAM && sp_idx == 127 && parameter_done) begin
      state <= STATE_SPIDLE;
      // sp_cycle <= 0;
    end
    else if (state == STATE_SPPARAM) begin
      sp_cycle <= sp_cycle + 1;
    end
    else if (state == STATE_SPPIPELINE && sp_idx != 127 && pipeline_done) begin
      state <= STATE_SPPARAM;
      sp_cycle <= 0;
      sp_idx <= sp_idx + 1;
    end
    else if (state == STATE_SPPIPELINE && sp_idx == 127 && pipeline_done) begin
      state <= STATE_SPIDLE;
      // sp_cycle <= 0;
    end
    else if (state == STATE_SPPIPELINE) begin
      sp_cycle <= sp_cycle + 1;
    end
    else if (state == STATE_SPIDLE) begin
    end
    else begin
      // unreachable
      // state <= STATE_WAITSYNC;
      // sp_cycle <= 0;
      // sp_idx <= 0;
    end
  end
  else begin
    state <= STATE_WAITSYNC;
    sp_cycle <= 0;
    sp_idx <= 0;
  end
end

vpu_sp_parameter_load vpu_sp_parameter_load (
  clk,
  rst_n,
  parameter_enable,
  sp_idx,
  y,
  param_en,
  param_addr,
  param_dout,
  sp_data0_cache,
  sp_data1_cache,
  sp_affine0_cache,
  sp_affine1_cache,
  sp_affine2_cache,
  parameter_done
);

logic [31:0] color_debug;
vpu_sp_pipeline vpu_sp_pipeline (
  clk,
  rst_n,
  pipeline_enable,
  sp_idx,
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
  line_web,
  line_bankb,
  line_addrb,
  line_dinb,
  line_doutb,
  color_debug,
  pipeline_done
);

endmodule

/* -------------------------------- */
module vpu_sp_parameter_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        parameter_enable,
  input  logic [ 6:0] sp_idx,
  input  logic [ 7:0] y,
  output logic        param_en,
  output logic [ 9:0] param_addr,
  input  logic [31:0] param_dout,
  output logic  [31:0] sp_data0_cache,
  output logic  [31:0] sp_data1_cache,
  output logic  [31:0] sp_affine0_cache,
  output logic  [31:0] sp_affine1_cache,
  output logic  [31:0] sp_affine2_cache,
  output logic         done
);

localparam PARAM_SIZE = 5;

logic parameter_enable_prev;
logic [2:0] offset;
logic [2:0] offset_prev;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    offset <= 0;
  end
  else if (done) begin
    offset <= 0;
  end
  else if (~parameter_enable) begin
    offset <= 0;
  end
  else begin
    if (offset < PARAM_SIZE-1) begin
      offset <= offset +1;
    end
  end

  parameter_enable_prev <= parameter_enable;
  offset_prev <= offset;
end

always_comb begin
  if (parameter_enable_prev) begin
    case(offset_prev)
      0: sp_data0_cache   = param_dout;
      1: sp_data1_cache   = param_dout;
      2: sp_affine0_cache = param_dout;
      3: sp_affine1_cache = param_dout;
      4: sp_affine2_cache = param_dout;
    endcase
  end
end

logic [ 0: 0]/*[ 0: 0]*/ sp_enable;
logic [ 0: 0]/*[ 1: 1]*/ sp_afen;
logic [ 1: 0]/*[ 7: 6]*/ sp_tilesize;
logic [ 7: 0]/*[31:24]*/ sp_y;
logic [ 8: 0]/*[1+31:24]*/ sp_y1;
logic [ 7: 0] th;
logic [ 0: 0] area_in;
assign sp_enable   = sp_data0_cache[ 0: 0];
assign sp_afen     = sp_data0_cache[ 1: 1];
assign sp_tilesize = sp_data0_cache[ 7: 6];
assign sp_y        = sp_data0_cache[31:24];
assign th = get_th(sp_tilesize);
assign sp_y1       = sp_y + th;
assign area_in = sp_enable && 
                (sp_afen ?
                  (((sp_y - (th>>1)) <= y) && (y < (sp_y1 + (th>>1)))) :
                  ((sp_y <= y) && (y < sp_y1)));

assign done = (~rst_n) ? 0 :
  (~area_in && offset == 1) ? 1 :
  (area_in && offset == PARAM_SIZE-1) ? 1 : 0;
  // (area_in) ? 0 : done;

assign param_addr = (sp_idx * PARAM_SIZE) + offset;
assign param_en = parameter_enable;
// assign param_en = parameter_enable | parameter_enable_prev;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        pipeline_enable,
  input  logic [ 6:0] sp_idx,
  input  logic [ 7:0] y,
  input  logic  [31:0] sp_data0_cache,
  input  logic  [31:0] sp_data1_cache,
  input  logic  [31:0] sp_affine0_cache,
  input  logic  [31:0] sp_affine1_cache,
  input  logic  [31:0] sp_affine2_cache,
  output logic                 tile_en,
  output logic [TILE_ADDR_W-1:0] tile_addr,
  input  logic [TILE_DATA_W-1:0] tile_data,
  output logic                 pal_en,
  output logic [PAL_ADDR_W-1:0]  pal_addr,
  input  logic [PAL_DATA_W-1:0]  pal_data,
  output logic [LINEBUFF_BANK_W-1:0] line_web,
  output logic [LINEBUFF_BANK_W-1:0] line_bankb,
  output logic [LINEBUFF_ADDR_W-1:0] line_addrb,
  output logic  [LINEBUFF_DATA_W-1:0] line_dinb,
  input  logic [LINEBUFF_DATA_W-1:0] line_doutb,
  output logic [31:0] color,
  output logic        pipeline_done
);

logic [1:0] layer;

logic [ 0: 0]/*[ 0: 0]*/ sp_enable;
logic [ 0: 0]/*[ 1: 1]*/ sp_afen;
logic [ 1: 0]/*[ 3: 2]*/ sp_layer;
logic [ 0: 0]/*[ 4: 4]*/ sp_hflip;
logic [ 0: 0]/*[ 5: 5]*/ sp_vflip;
logic [ 1: 0]/*[ 7: 6]*/ sp_tilesize;
// logic [ 0: 0]/*[14: 8]*/ sp__reserved;
logic [ 8: 0]/*[23:15]*/ sp_x;
logic [ 7: 0]/*[31:24]*/ sp_y;

logic [ 3: 0]/*[ 3: 0]*/ sp_tile_bank;
logic [11: 0]/*[15: 4]*/ sp_tile_idx;
//logic [ 3: 0]/*[19:16]*/ sp__palreserved0;
logic [ 3: 0]/*[23:20]*/ sp_pal_transparency;
//logic [ 0: 0]/*[24:24]*/ sp__palreserved1;
logic [ 0: 0]/*[25:25]*/ sp_pal_mode;
logic [ 1: 0]/*[27:26]*/ sp_pal_bank;
logic [ 3: 0]/*[31:28]*/ sp_pal_no;

logic [95: 0] sp_affine;

assign sp_enable   = sp_data0_cache[ 0: 0];
assign sp_afen     = sp_data0_cache[ 1: 1];
assign sp_layer    = sp_data0_cache[ 3: 2];
assign sp_hflip    = sp_data0_cache[ 4: 4];
assign sp_vflip    = sp_data0_cache[ 5: 5];
assign sp_tilesize = sp_data0_cache[ 7: 6];
// assign sp__reserved = sp_data0_cache[14: 8];
assign sp_x        = sp_data0_cache[23:15];
assign sp_y        = sp_data0_cache[31:24];

assign sp_tile_bank        = sp_data1_cache[ 3: 0];
assign sp_tile_idx         = sp_data1_cache[15: 4];
//assign sp__palreserved0    = sp_data1_cache[19:16];
assign sp_pal_transparency = sp_data1_cache[23:20];
//assign sp__palreserved1    = sp_data1_cache[24:24];
assign sp_pal_mode         = sp_data1_cache[25:25];
assign sp_pal_bank         = sp_data1_cache[27:26];
assign sp_pal_no           = sp_data1_cache[31:28];

assign sp_affine = {sp_affine0_cache, sp_affine1_cache, sp_affine2_cache};

logic [8:0] x;
logic [8:0] objx;
logic [7:0] objy;
logic [8:0] xbegin;
logic [8:0] xend;
logic [7:0] tw;
logic [7:0] th;
logic       area_in;
logic       area_inA;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    // x <= 0;
    x <= sp_x;
  end
  else begin
    if (pipeline_enable) begin
      // if (x < tw-1) begin
      // if (x < (sp_afen ? (sp_x + tw-1) : (sp_x + tw + (tw>>2)-1))) begin
      if (x < (sp_afen ? (sp_x + tw + (tw>>1)-1) : (sp_x + tw-1))) begin
        x <= x + 1;
      end else begin
      end
    end
    else begin
      // x <= sp_afen ? sp_x : sp_x - (tw>>2);
      x <= sp_afen ? sp_x - (tw>>1) : sp_x;
    end
  end
end

vpu_sp_pipeline0_affine_transform sp_pipe0(
  clk,
  rst_n,
  x,
  y,
  sp_tilesize,
  sp_x,
  sp_y,
  sp_enable,
  sp_afen,
  sp_affine,
  objx,
  objy,
  xbegin,
  xend,
  tw,
  th,
  area_in,
  area_inA
);

logic       sp_enable_p02;
logic       area_in_p02;
logic       area_inA_p02;
logic [1:0] layer_p02;
logic [8:0] x_p02;
logic [7:0] y_p02;
logic [8:0] objx_p02;
logic [7:0] objy_p02;
logic [7:0] tw_p02;
logic [7:0] th_p02;
logic [3:0]  tile_bank_p02;
logic [15:0] tile_idx_p02;
logic [0:0] pal_mode_p02;
logic [1:0] pal_bank_p02;
logic [3:0] pal_no_p02;
assign area_in_p02 = area_in;
assign area_inA_p02 = area_inA;
// assign x_p02 = x;
// assign y_p02 = y;
assign objx_p02 = objx;
assign objy_p02 = objy;
assign tw_p02 = tw;
assign th_p02 = th;
always_ff @(posedge clk) begin
  sp_enable_p02 <= sp_enable;
  // area_in_p02 <= area_in;
  // area_inA_p02 <= area_inA;
  layer_p02 <= sp_layer;
  x_p02 <= x;
  y_p02 <= y;
  // objx_p02 <= objx;
  // objy_p02 <= objy;
  // tw_p02 <= tw;
  // th_p02 <= th;
  tile_bank_p02 <= sp_tile_bank;
  tile_idx_p02 <= {4'b0000, sp_tile_idx};
  pal_mode_p02 <= sp_pal_mode;
  pal_bank_p02 <= sp_pal_bank;
  pal_no_p02 <= sp_pal_no;
end

logic [7:0] pal_idx;
vpu_sp_pipeline2_tile_load sp_pipe2 (
  clk,
  rst_n,
  objx_p02,
  objy_p02,
  tw_p02,
  tile_bank_p02,
  tile_idx_p02,
  tile_en,
  tile_addr,
  tile_data,
  pal_idx
);

logic       sp_enable_p23;
logic       area_in_p23;
logic       area_inA_p23;
logic [1:0] layer_p23;
logic [8:0] x_p23;
logic [8:0] objx_p23;
logic [7:0] tw_p23;
logic [7:0] pal_idx_p23;
logic [0:0] pal_mode_p23;
logic [1:0] pal_bank_p23;
logic [3:0] pal_no_p23;
assign pal_idx_p23 = pal_idx;
always_ff @(posedge clk) begin
  sp_enable_p23 <= sp_enable_p02;
  area_in_p23 <= area_in_p02;
  area_inA_p23 <= area_inA_p02;
  layer_p23 <= layer_p02;
  x_p23 <= x_p02;
  objx_p23 <= objx_p02;
  tw_p23 <= tw_p02;
  pal_mode_p23 <= pal_mode_p02;
  pal_bank_p23 <= pal_bank_p02;
  pal_no_p23 <= pal_no_p02;
  // pal_idx_p23 <= pal_idx;
end

logic [31:0] color_n;
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

logic       sp_enable_p34;
logic       area_in_p34;
logic       area_inA_p34;
logic [1:0] layer_p34;
logic [8:0] x_p34;
logic [8:0] objx_p34;
logic [7:0] tw_p34;
logic [31:0] color_n_p34;
assign color_n_p34 = color_n;
always_ff @(posedge clk) begin
  sp_enable_p34 <= sp_enable_p23;
  area_in_p34 <= area_in_p23;
  area_inA_p34 <= area_inA_p23;
  layer_p34 <= layer_p23;
  x_p34 <= x_p23;
  objx_p34 <= objx_p23;
  tw_p34 <= tw_p23;
  // color_n_p34 <= color_n;
end

logic [31:0] color_out;
logic done;
vpu_sp_pipeline4_color_merge sp_pipe4 (
  clk,
  rst_n,
  sp_enable_p34,
  // area_in_p34,
  area_inA_p34,
  layer_p34,
  x_p34,
  y,
  objx_p34,
  tw_p34,
  color_n_p34,
  line_web,
  line_bankb,
  line_addrb,
  line_dinb,
  line_doutb,
  done
);

assign pipeline_done = done;
assign color = line_doutb;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline0_affine_transform
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic [ 8:0] xin,
  input  logic [ 7:0] yin,
  input  logic [ 1:0] sp_tilesize,
  input  logic [ 8:0] sp_x,
  input  logic [ 7:0] sp_y,
  input  logic        sp_enable,
  input  logic        sp_afen,
  input  logic [95:0] sp_affine,
  output logic  [ 8:0] objx,
  output logic  [ 7:0] objy,
  output logic  [ 8:0] xbegin,
  output logic  [ 8:0] xend,
  output logic  [ 7:0] tw,
  output logic  [ 7:0] th,
  output logic         area_in,
  output logic         area_inA
);

shortint objx0;
shortint objy0;
shortint x0, y0, Ba, Bb, Bc, Bd, Bx, By;
int Bax0, Bby0, Bcx0, Bdy0;
shortint x1, y1;
byte tw0, th0;
logic area_in1;
logic area_inA1;

assign Bb = sp_affine[16*6-1:16*5];
assign Ba = sp_affine[16*5-1:16*4];
assign Bd = sp_affine[16*4-1:16*3];
assign Bc = sp_affine[16*3-1:16*2];
assign By = sp_affine[16*2-1:16*1];
assign Bx = sp_affine[16*1-1:16*0];

always_ff @(posedge clk) begin
  objx <= x1;
  objy <= y1;
  tw <= get_tw(sp_tilesize);
  th <= get_th(sp_tilesize);
  area_in <= area_in1;
  area_inA <= area_inA1;
end

always_comb begin
  objx0 = shortint'(xin) - shortint'(sp_x);
  objy0 = shortint'(yin) - shortint'(sp_y);
  tw0 = get_tw(sp_tilesize);
  th0 = get_th(sp_tilesize);
  xbegin = xin;
  xend = xin + tw;
  // [TODO] affine transform
  if (sp_afen) begin
    area_in1 = sp_enable && (-tw0 <= objx0 && objx0 < tw0+(tw0>>2)) && (-th0 <= objy0 && objy0 < th0+(th0>>2));
    // x0 = {7'h0, objx0} - Bx;
    // y0 = {8'h0, objy0} - By;
    x0 = objx0 - Bx;
    y0 = objy0 - By;
    Bax0 = Ba * x0;
    Bby0 = Bb * y0;
    Bcx0 = Bc * x0;
    Bdy0 = Bd * y0;
    x1 = (Bax0 >> 8) + (Bby0 >> 8) + Bx;
    y1 = (Bcx0 >> 8) + (Bdy0 >> 8) + By;
    area_inA1 = sp_enable && (0 <= x1 && x1 < tw0) && (0 <= y1 && y1 < th0);
    xbegin = xbegin - (tw>>2);
    xend   = xend   + (tw>>2);
  end
  else begin
    area_in1 = sp_enable && (0 <= objx0 && objx0 < tw0) && (0 <= objy0 && objy0 < th0);
    x1 = objx0;
    y1 = objy0;
    area_inA1 = sp_enable && (0 <= x1 && x1 < tw0) && (0 <= y1 && y1 < th0);
  end
end

endmodule

/* -------------------------------- */
module vpu_sp_pipeline2_tile_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic [ 8:0] objx,
  input  logic [ 7:0] objy,
  input  logic [ 7:0] tw,
  input  logic [TILE_BANK_W-1:0] sp_tile_bank,
  input  logic [TILE_INDX_W-1:0] sp_tile_idx,
  output logic        tile_en,
  output logic  [TILE_ADDR_W-1:0] tile_addr,
  input  logic  [TILE_DATA_W-1:0] tile_data,
  output logic  [PAL_INDX_W-1:0] pal_idx
);

localparam HW_TILE_W = 8;
localparam HW_TILE_H = 8;
localparam HW_TILEMAP_W = 512;
localparam HW_TILEMAP_H = 512;


shortint offset;
shortint tilex;
shortint tiley;
logic  [ 8:0] tx0;
logic  [ 8:0] ty0;
logic  [TILE_INDX_W-1:0] tile_idx;
logic  [TILE_INDX_W-1:0] tile_addr_lo;

always_comb begin
  tx0 = objx & (HW_TILEMAP_W-1);
  ty0 = objy & (HW_TILEMAP_H-1);
  offset = (tx0 / HW_TILE_W) + (ty0 / HW_TILE_H) * (tw / HW_TILE_W);
  tile_idx = sp_tile_idx + offset;

  tilex = (objx & (HW_TILEMAP_W-1)) & (HW_TILE_W-1);
  tiley = (objy & (HW_TILEMAP_H-1)) & (HW_TILE_H-1);
  tile_addr_lo = ((tile_idx * HW_TILE_W * HW_TILE_H) + (tiley * HW_TILE_W) + tilex);
  tile_addr = {sp_tile_bank, tile_addr_lo};
end

assign tile_en = 1;
assign pal_idx = tile_data;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline3_palette_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic [ 1:0] layer,
  input  logic        pal_mode,
  input  logic [PAL_BANK_W-1:0] pal_bank,
  input  logic [ 3:0] pal_no,
  input  logic [PAL_INDX_W-1:0] pal_idx,
  output logic        pal_en,
  output logic  [PAL_ADDR_W-1:0] pal_addr,
  input  logic  [PAL_DATA_W-1:0] pal_data,
  output logic  [31:0] color_n
);

assign pal_addr = (pal_mode == 0) ? {pal_bank, pal_idx} : {pal_bank, pal_no, pal_idx[3:0]};
assign color_n = pal_data;
assign pal_en = 1;

endmodule

/* -------------------------------- */
module vpu_sp_pipeline4_color_merge
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        sp_enable,
  input  logic        area_inA,
  input  logic [ 1:0] layer,
  input  logic [ 8:0] x,
  input  logic [ 7:0] y,
  input  logic [ 8:0] objx,
  input  logic [ 7:0] tw,
  input  logic [31:0] color_n,
  output logic                        line_web,
  output logic [LINEBUFF_BANK_W-1:0] line_bankb,
  output logic  [LINEBUFF_ADDR_W-1:0] line_addrb,
  output logic  [LINEBUFF_DATA_W-1:0] line_dinb,
  input  logic [LINEBUFF_DATA_W-1:0] line_doutb,
  output logic         done
);

function logic [31:0] color_merge(logic [31:0] col_buf, logic [31:0] col_n);
  logic [31:0] dst;
  logic [31:0] src;
  logic [ 7:0] src_a;
  dst = col_buf;
  // for (int i=0; i<4; i++) begin
    src = col_n;
    src_a = src[31:24];
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
  // end
  return dst;
endfunction

assign line_bankb = y[0]; // y & 1'b1;
assign line_web = sp_enable & area_inA;
assign line_addrb = x;
assign line_dinb = color_merge(line_doutb, color_n);

assign done = sp_enable & area_inA & (objx == tw-1);

endmodule
