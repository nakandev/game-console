module vpu_bg
  import gameconsole_pkg::*;
(
  input  logic                   clk,
  input  logic                   rst_n,

  input  logic [10:0]            line_cycle,
  input  logic [8:0]             y,

  output logic                   param_en,
  output logic [PRM_ADDR_W-1:0]  param_addr,
  input  logic [PRM_DATA_W-1:0]  param_dout,

  output logic                   map_en,
  output logic [MAP_ADDR_W-1:0]  map_addr,
  input  logic [MAP_DATA_W-1:0]  map_dout,

  output logic                   tile_en,
  output logic [TILE_ADDR_W-1:0] tile_addr,
  input  logic [TILE_DATA_W-1:0] tile_dout,

  output logic                   pal_en,
  output logic [PAL_ADDR_W-1:0]  pal_addr,
  input  logic [PAL_DATA_W-1:0]  pal_dout,

  output logic [LINEBUFF_BANK_W-1:0] line_wea,
  output logic [LINEBUFF_ADDR_W-1:0] line_addra,
  output logic  [LINEBUFF_DATA_W-1:0] line_dina,
  input  logic [LINEBUFF_DATA_W-1:0] line_douta,

  // output logic                   bg_param,
  output logic                   dot_clk,
  output logic  [31:0]            color,
  output logic            hdraw,
  output logic            vdraw
  // output logic            hdraw,
  // output logic            vdraw
);

localparam HMAX = SCREEN_W + SCREEN_HBLANK;
localparam VMAX = SCREEN_H + SCREEN_VBLANK;
localparam LINE_CYCLE_PARAM = 20;
localparam LINE_CYCLE_VISIBLE = LINE_CYCLE_PARAM + SCREEN_W * 4;
localparam LINE_CYCLE_MAX = LINE_CYCLE_PARAM + HMAX * 4;

logic [31:0] bg_data0_cache[4];
logic [31:0] bg_data1_cache[4];
logic [31:0] bg_affine0_cache[4];
logic [31:0] bg_affine1_cache[4];
logic [31:0] bg_affine2_cache[4];

logic [8:0] x;

logic pipeline_enable;

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

logic is_parameter_fetch;
assign is_parameter_fetch = (state == STATE_PARAM);
// assign bg_param = is_parameter_fetch;
vpu_bg_parameter_load vpu_bg_parameter_load (
  clk,
  rst_n,
  is_parameter_fetch,
  param_en,
  param_addr,
  param_dout,
  bg_data0_cache,
  bg_data1_cache,
  bg_affine0_cache,
  bg_affine1_cache,
  bg_affine2_cache
);

logic [31:0] color_debug;
assign pipeline_enable = (state == STATE_PIPELINE);
logic dummy;
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

assign hdraw = (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
assign vdraw = y < SCREEN_H;
// localparam VH_DELAY = 16;
// logic [0:0] vdelay[VH_DELAY];
// logic [0:0] hdelay[VH_DELAY];
// always_ff @(posedge clk) begin
//   hdelay[0] <= (state == STATE_PIPELINE) && (line_cycle < LINE_CYCLE_VISIBLE);
//   vdelay[0] <= y < SCREEN_H;
//   for (int i=1; i<VH_DELAY; i++) begin
//     hdelay[i] <= hdelay[i-1];
//     vdelay[i] <= vdelay[i-1];
//   end
//   // hdraw <= hdelay[10-1];
//   // vdraw <= vdelay[10-1];
//   hdraw <= hdelay[12-1];
//   vdraw <= vdelay[12-1];
//   // hdraw <= hdelay[16-1];
//   // vdraw <= vdelay[16-1];
// end

endmodule

/* -------------------------------- */
module vpu_bg_parameter_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        is_parameter_fetch,
  output logic        bg_en,
  output logic [ 9:0] bg_addr,
  input  logic [31:0] bg_dout,
  output logic  [31:0] bg_data0_cache[4],
  output logic  [31:0] bg_data1_cache[4],
  output logic  [31:0] bg_affine0_cache[4],
  output logic  [31:0] bg_affine1_cache[4],
  output logic  [31:0] bg_affine2_cache[4]
);

localparam PARAM_SIZE = 5;
localparam SPRITE_RAM_BG_BASE = 128 * PARAM_SIZE;

// logic [1:0] layer;
// logic [2:0] offset;
// logic [2:0] offset_prev;
// 
// always_ff @(posedge clk) begin
//   if (~rst_n) begin
//     layer <= 0;
//     offset <= 0;
//   end
//   else if (~is_parameter_fetch) begin
//     layer <= 0;
//     offset <= 0;
//   end
//   else begin
//     offset <= (offset < PARAM_SIZE-1) ? offset + 1 : 0;
//     if (offset == PARAM_SIZE-1) begin
//       layer <= (layer < 4-1) ? layer + 1 : 0;
//     end
//   end
//   offset_prev <= offset;
// end
// 
// always_comb begin
//   if (is_parameter_fetch) begin
//     case(offset_prev)
//       0: bg_data0_cache  [layer] = bg_dout;
//       1: bg_data1_cache  [layer] = bg_dout;
//       2: bg_affine0_cache[layer] = bg_dout;
//       3: bg_affine1_cache[layer] = bg_dout;
//       4: bg_affine2_cache[layer] = bg_dout;
//     default: ;
//     endcase
//   end
// end
// 
// assign bg_addr = SPRITE_RAM_BG_BASE + (layer * PARAM_SIZE) + offset;
// assign bg_en = is_parameter_fetch;
// // assign bg_en = 1;


// -------- Type B --------
logic [1:0] layer;
logic [2:0] offset;
logic [4:0] count;
logic [4:0] count_prev;

assign layer = count_prev / PARAM_SIZE;
assign offset = count_prev % PARAM_SIZE;
assign bg_en = is_parameter_fetch;
assign bg_addr = SPRITE_RAM_BG_BASE + count;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    count <= 0;
    count_prev <= 0;
  end
  else begin
    count_prev <= count;
    if (~is_parameter_fetch) begin
      count <= 0;
    end
    else begin
      if (count == PARAM_SIZE*4-1) begin
        count <= 0;
      end
      else begin
        count <= count + 1;
      end
      // if (bg_en) begin
        case(offset)
          0: bg_data0_cache  [layer] <= bg_dout;
          1: bg_data1_cache  [layer] <= bg_dout;
          2: bg_affine0_cache[layer] <= bg_dout;
          3: bg_affine1_cache[layer] <= bg_dout;
          4: bg_affine2_cache[layer] <= bg_dout;
        default: ;
        endcase
      // end
    end
  end
end


endmodule

/* -------------------------------- */
module vpu_bg_pipeline
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        pipeline_enable,
  input  logic [ 7:0] y,
  input  logic  [31:0] bg_data0_cache[4],
  input  logic  [31:0] bg_data1_cache[4],
  input  logic  [31:0] bg_affine0_cache[4],
  input  logic  [31:0] bg_affine1_cache[4],
  input  logic  [31:0] bg_affine2_cache[4],
  output logic                 map_en,
  output logic [MAP_ADDR_W-1:0]  map_addr,
  input  logic [MAP_DATA_W-1:0]  map_data,
  output logic                 tile_en,
  output logic [TILE_ADDR_W-1:0] tile_addr,
  input  logic [TILE_DATA_W-1:0] tile_data,
  output logic                 pal_en,
  output logic [PAL_ADDR_W-1:0]  pal_addr,
  input  logic [PAL_DATA_W-1:0]  pal_data,
  output logic [LINEBUFF_BANK_W-1:0] line_wea,
  output logic [LINEBUFF_ADDR_W-1:0] line_addra,
  output logic  [LINEBUFF_DATA_W-1:0] line_dina,
  input  logic [LINEBUFF_DATA_W-1:0] line_douta,
  output logic [31:0] color,
  output logic        dummy
);

logic [1:0] layer;

logic [8:0] x;
logic [ 0: 0]/*[31:31]*/ bg_enable;
logic [ 0: 0]/*[30:30]*/ bg_afen;
logic [ 1: 0]/*[29:28]*/ bg_layer;
logic [ 0: 0]/*[27:27]*/ bg_hflip;
logic [ 0: 0]/*[26:26]*/ bg_vflip;
logic [ 1: 0]/*[25:24]*/ bg_tilesize;
logic [ 0: 0]/*[23:23]*/ bg_mode;
// logic [ 5: 0]/*[22:17]*/ bg__reserved;
logic [ 8: 0]/*[16: 8]*/ bg_x;
logic [ 7: 0]/*[ 7: 0]*/ bg_y;

logic [ 3: 0]/*[31:28]*/ bg_tile_bank;
logic [ 3: 0]/*[27:24]*/ bg_map_bank;
//logic [ 7: 0]/*[23:16]*/ bg__tilereserved;
//logic [ 3: 0]/*[15:12]*/ bg__palreserved0;
logic [ 3: 0]/*[11: 8]*/ bg_pal_transparency;
//logic [ 0: 0]/*[ 7: 7]*/ bg__palreserved1;
logic [ 0: 0]/*[ 6: 6]*/ bg_pal_mode;
logic [ 1: 0]/*[ 5: 4]*/ bg_pal_bank;
logic [ 3: 0]/*[ 3: 0]*/ bg_pal_no;

logic [95: 0] bg_affine;

assign bg_enable   = bg_data0_cache[layer][31:31];
assign bg_afen     = bg_data0_cache[layer][30:30];
assign bg_layer    = bg_data0_cache[layer][29:28];
assign bg_hflip    = bg_data0_cache[layer][27:27];
assign bg_vflip    = bg_data0_cache[layer][26:26];
assign bg_tilesize = bg_data0_cache[layer][25:24];
assign bg_mode     = bg_data0_cache[layer][23:23];
assign bg_x        = bg_data0_cache[layer][16: 8];
assign bg_y        = bg_data0_cache[layer][ 7: 0];

assign bg_tile_bank        = bg_data1_cache[layer][31:28];
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

logic [8:0] objx;
logic [7:0] objy;
logic [8:0] xbegin;
logic [8:0] xend;
logic [7:0] tw;
logic [7:0] th;
vpu_bg_pipeline0_affine_transform bg_pipe0(
  clk,
  rst_n,
  x,
  y,
  bg_enable,
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

logic       bg_enable_p01;
logic [8:0] x_p01;
logic [1:0] layer_p01;
logic [8:0] objx_p01;
logic [7:0] objy_p01;
logic [7:0] tw_p01;
logic [7:0] th_p01;
logic [3:0] tile_bank_p01;
logic [3:0] map_bank_p01;
logic [0:0] pal_mode_p01;
logic [1:0] pal_bank_p01;
logic [3:0] pal_no_p01;
assign objx_p01 = objx;
assign objy_p01 = objy;
assign tw_p01 = tw;
assign th_p01 = th;
always_ff @(posedge clk) begin
  bg_enable_p01 <= bg_enable;
  x_p01 <= x;
  layer_p01 <= layer;
  // objx_p01 <= objx;
  // objy_p01 <= objy;
  // tw_p01 <= tw;
  // th_p01 <= th;
  tile_bank_p01 <= bg_tile_bank;
  map_bank_p01 <= bg_map_bank;
  pal_mode_p01 <= bg_pal_mode;
  pal_bank_p01 <= bg_pal_bank;
  pal_no_p01 <= bg_pal_no;
end

// logic [8:0] tx0;
// logic [8:0] ty0;
logic [15:0] tile_idx;
vpu_bg_pipeline1_map_load bg_pipe1 (
  clk,
  rst_n,
  bg_enable_p01,
  objx_p01,
  objy_p01,
  tw_p01,
  th_p01,
  // tx0,
  // ty0,
  map_bank_p01,
  map_en,
  map_addr,
  map_data,
  tile_idx
);

logic       bg_enable_p12;
logic [8:0] x_p12;
logic [1:0] layer_p12;
logic [8:0] objx_p12;
logic [7:0] objy_p12;
logic [3:0] tile_bank_p12;
// logic [15:0] tile_idx_p12;
logic [0:0] pal_mode_p12;
logic [1:0] pal_bank_p12;
logic [3:0] pal_no_p12;
always_ff @(posedge clk) begin
  bg_enable_p12 <= bg_enable_p01;
  x_p12 <= x_p01;
  layer_p12 <= layer_p01;
  objx_p12 <= objx_p01;
  objy_p12 <= objy_p01;
  tile_bank_p12 <= tile_bank_p01;
  // tile_idx_p12 <= tile_idx;
  pal_mode_p12 <= pal_mode_p01;
  pal_bank_p12 <= pal_bank_p01;
  pal_no_p12 <= pal_no_p01;
end

logic [7:0] pal_idx;
vpu_bg_pipeline2_tile_load bg_pipe2 (
  clk,
  rst_n,
  bg_enable_p12,
  objx_p12,
  objy_p12,
  tile_bank_p12,
  tile_idx,
  tile_en,
  tile_addr,
  tile_data,
  pal_idx
);

logic       bg_enable_p23;
logic [8:0] x_p23;
logic [1:0] layer_p23;
logic [7:0] pal_idx_p23;
logic [0:0] pal_mode_p23;
logic [1:0] pal_bank_p23;
logic [3:0] pal_no_p23;
assign pal_idx_p23 = pal_idx;
always_ff @(posedge clk) begin
  bg_enable_p23 <= bg_enable_p12;
  x_p23 <= x_p12;
  layer_p23 <= layer_p12;
  pal_mode_p23 <= pal_mode_p12;
  pal_bank_p23 <= pal_bank_p12;
  pal_no_p23 <= pal_no_p12;
  // pal_idx_p23 <= pal_idx;
end

logic [31:0] color_n[4];
vpu_bg_pipeline3_palette_load  bg_pipe3 (
  clk,
  rst_n,
  bg_enable_p23,
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

// logic       bg_enable_p34[4];
// logic [3:0] bg_enable_p34;
logic [8:0] x_p34;
logic [1:0] layer_p34;
// logic [31:0] color_n_p34[4];
logic [31:0] color_n_p34[4];
assign color_n_p34 = color_n;
always_ff @(posedge clk) begin
  // if (~pipeline_enable) begin
  //   // for (int i=0; i<4; i++) bg_enable_p34[i] <= 0;
  // end
  // else begin
  //   // bg_enable_p34[layer_p23] <= bg_enable_p23;
    x_p34 <= x_p23;
    layer_p34 <= layer_p23;
    // color_n_p34 <= color_n;
  // end
end

logic [31:0] color_out;
logic done;
vpu_bg_pipeline4_color_merge bg_pipe4 (
  clk,
  rst_n,
  // bg_enable_p34,
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
  input  logic        clk,
  input  logic        rst_n,
  input  logic [ 8:0] xin,
  input  logic [ 7:0] yin,
  input  logic        bg_enable,
  input  logic [ 1:0] bg_tilesize,
  input  logic [ 8:0] bg_x,
  input  logic [ 7:0] bg_y,
  input  logic        bg_afen,
  input  logic [95:0] bg_affine,
  output logic  [ 8:0] objx,
  output logic  [ 7:0] objy,
  output logic  [ 8:0] xbegin,
  output logic  [ 8:0] xend,
  output logic  [ 7:0] tw,
  output logic  [ 7:0] th
);

logic [8:0] lobjx;
logic [7:0] lobjy;

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

// always_comb begin
always_ff @(posedge clk) begin
  objx <= xin - bg_x;
  objy <= yin - bg_y;
  tw   <= get_tw(bg_tilesize);
  th   <= get_th(bg_tilesize);
  // [TODO] affine transform
  // if (bg_afen) begin
  // end
end

endmodule

/* -------------------------------- */
module vpu_bg_pipeline1_map_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        bg_enable,
  input  logic [ 8:0] objx,
  input  logic [ 7:0] objy,
  input  logic [ 7:0] tw,
  input  logic [ 7:0] th,
  // output logic  [ 8:0] tx0,
  // output logic  [ 8:0] ty0,
  input  logic [ 3:0] map_bank,
  output logic        map_en,
  output logic [MAP_ADDR_W-1:0] map_addr,
  input  logic [MAP_DATA_W-1:0] map_data,
  output logic [15:0] tile_idx
);

shortint offset0;
shortint offset1x;
shortint offset1y;

logic        bg_enable_prev;
logic [ 7:0] tw_prev;
logic [ 7:0] th_prev;
logic [ 8:0] tx0_prev;
logic [ 8:0] ty0_prev;

assign map_en = bg_enable;
// assign map_addr = {map_bank, offset0[10:0]};  // debug
assign offset0 = 
  (map_bank << MAP_INDX_W) +
  ((objx & (HW_TILEMAP_W-1)) / tw) +
  ((objy & (HW_TILEMAP_H-1)) / th) * (HW_TILEMAP_W / tw);
assign map_addr = offset0[MAP_ADDR_W-1:0];


always_ff @(posedge clk) begin
  bg_enable_prev <= bg_enable;
  tw_prev <= tw;
  th_prev <= th;
  tx0_prev <= objx & (HW_TILEMAP_W-1);
  ty0_prev <= objy & (HW_TILEMAP_W-1);
end

assign tile_idx = bg_enable_prev ? (map_data *
  (tw_prev * th_prev / HW_TILE_H / HW_TILE_W) +
  ((tx0_prev & (tw_prev-1)) / HW_TILE_W) +
  ((ty0_prev & (th_prev-1)) / HW_TILE_H * (tw_prev / HW_TILE_W)))
  : 0;


endmodule

/* -------------------------------- */
module vpu_bg_pipeline2_tile_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        bg_enable,
  input  logic [ 8:0] objx,
  input  logic [ 7:0] objy,
  input  logic [TILE_BANK_W-1:0] tile_bank,
  input  logic [TILE_INDX_W-1:0] tile_idx,
  output logic        tile_en,
  output logic [TILE_ADDR_W-1:0] tile_addr,
  input  logic [TILE_DATA_W-1:0] tile_data,
  output logic [PAL_INDX_W-1:0] pal_idx
);

logic bg_enable_prev;
// shortint tilex;
// shortint tiley;

assign tile_en = bg_enable;
// assign tile_addr = (tile_bank << TILE_INDX_W) + ((tile_idx * HW_TILE_W * HW_TILE_H) + (tiley * HW_TILE_W) + tilex);
assign tile_addr = (tile_bank << TILE_INDX_W) +
  ((tile_idx * HW_TILE_W * HW_TILE_H) +
  (((objy & (HW_TILEMAP_H-1)) & (HW_TILE_H-1)) * HW_TILE_W) +
  ((objx & (HW_TILEMAP_W-1)) & (HW_TILE_W-1)));


always_ff @(posedge clk) begin
  bg_enable_prev <= bg_enable;
end

assign pal_idx = bg_enable_prev ? tile_data : 0;

endmodule

/* -------------------------------- */
module vpu_bg_pipeline3_palette_load
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  input  logic        bg_enable,
  input  logic [ 1:0] layer,
  input  logic        pal_mode,
  input  logic [PAL_BANK_W-1:0] pal_bank,
  input  logic [ 3:0] pal_no,
  input  logic [PAL_INDX_W-1:0] pal_idx,
  output logic        pal_en,
  output logic [PAL_ADDR_W-1:0] pal_addr,
  input  logic [PAL_DATA_W-1:0] pal_data,
  output logic  [31:0] color_n[4]
);

logic bg_enable_prev;
logic [1:0] layer_prev;

assign pal_en = bg_enable;
// assign pal_addr = bg_enable ? 
//   ((pal_mode == 0) ? {pal_bank, pal_idx} : {pal_bank, pal_no[3:0], pal_idx[3:0]})
//   : 0;
assign pal_addr =  
  ((pal_mode == 0) ? {pal_bank, pal_idx} : {pal_bank, pal_no[3:0], pal_idx[3:0]})
  ;

always_ff @(posedge clk) begin
  bg_enable_prev <= bg_enable;
  layer_prev <= layer;
end

assign color_n[0] = (layer_prev == 0) ? bg_enable_prev ? pal_data : 32'h0 : color_n[0];
assign color_n[1] = (layer_prev == 1) ? bg_enable_prev ? pal_data : 32'h0 : color_n[1];
assign color_n[2] = (layer_prev == 2) ? bg_enable_prev ? pal_data : 32'h0 : color_n[2];
assign color_n[3] = (layer_prev == 3) ? bg_enable_prev ? pal_data : 32'h0 : color_n[3];
// always_comb begin
//   if (bg_enable_prev) begin
//     color_n[layer_prev] = pal_data;
//   end
// end

endmodule

/* -------------------------------- */
module vpu_bg_pipeline4_color_merge
  import gameconsole_pkg::*;
(
  input  logic        clk,
  input  logic        rst_n,
  // input  logic [3:0] bg_enable,
  input  logic [ 8:0] x,
  input  logic [ 1:0] layer,
  input  logic [31:0] color_n[4],
  output logic [LINEBUFF_BANK_W-1:0] line_wea,
  // output logic [LINEBUFF_ADDR_W-1:0] line_addra,
  output logic [LINEBUFF_ADDR_W-1:0] line_addra,
  output logic [LINEBUFF_DATA_W-1:0] line_dina,
  input  logic [LINEBUFF_DATA_W-1:0] line_douta,
  output logic  [31:0] color_out,
  output logic         done
);

function automatic logic [31:0] color_merge(logic [31:0] dst, logic [31:0] src);
  logic [ 7:0] src_a = src[31:24];
  if (src_a == 8'h00) begin
    // dst = dst;
    dst = {8'hff, dst[23:0]};
  end
  else
  if (src_a == 8'hff) begin
  // else begin
    // dst = src;
    dst = {8'hff, src[23:0]};
  end
  else begin
    dst[ 7: 0] = ((dst[ 7: 0] * (255 - src_a) + src[ 7: 0] * src_a) >> 8);
    dst[15: 8] = ((dst[15: 8] * (255 - src_a) + src[15: 8] * src_a) >> 8);
    dst[23:16] = ((dst[23:16] * (255 - src_a) + src[23:16] * src_a) >> 8);
    // dst[31:24] = (dst[31:24] + src[31:24]) / 2;
    dst[31:24] = 8'hff;
  end
  return dst;
endfunction

function automatic logic [31:0] color_merge_all(logic [31:0] cols[5]);
  logic [31:0] dst = 0;
  logic [31:0] src;
  logic [ 7:0] src_a;
  for (int i=0; i<5; i++) begin
    src = cols[i];
    src_a = src >> 24;
    if (src_a == 0) begin
      dst = {8'hff, dst[23:0]};
    end
    else if (src_a == 255) begin
    // else begin
      dst = {8'hff, src[23:0]};
    end
    else begin
      dst[ 7: 0] = ((dst[ 7: 0] * (255 - src_a) + src[ 7: 0] * src_a) / 256);
      dst[15: 8] = ((dst[15: 8] * (255 - src_a) + src[15: 8] * src_a) / 256);
      dst[23:16] = ((dst[23:16] * (255 - src_a) + src[23:16] * src_a) / 256);
      // dst[31:24] = (dst[31:24] + src[31:24]) / 2;
      dst[31:24] = 8'hff;
     end
  end
  return dst;
endfunction


logic [31:0] colors[5];

//-------- Trial A --------
assign done = (layer == 3);
assign line_wea = (layer == 3);  // read linebuffer pixel --> clear the pixel
// assign line_wea = 0;  // read linebuffer pixel --> clear the pixel
assign line_addra = x;
assign line_dina = 0;

assign colors[0] = color_n[0];
assign colors[1] = color_n[1];
assign colors[2] = color_n[2];
assign colors[3] = color_n[3];
assign colors[4] = line_douta;

// always_ff @(posedge clk) begin
//   if (layer == 3) begin
//     color_out <= color_merge_all(colors);
//   end
// end
always_comb begin
  if (layer == 3) begin
    color_out = color_merge_all(colors);
  end
end

//-------- Trial B --------
// assign done = (layer == 3);
// assign line_wea = (layer == 3);  // read linebuffer pixel --> clear the pixel
// assign line_addra = x;
// assign line_dina = 0;
// 
// logic [31:0] col_dst;
// logic [31:0] col_src;
// assign col_dst = layer == 0 ? 32'h0 : colors[layer - 1];
// assign col_src = bg_enable[layer] ? color_n[layer] : 32'h0;
// 
// always_comb begin
//   colors[layer] = color_merge(col_dst, col_src);
//   if (layer == 3) begin
//     colors[4] = color_merge(colors[3], line_douta);
//     color_out = colors[4];
//   end
// end

endmodule
