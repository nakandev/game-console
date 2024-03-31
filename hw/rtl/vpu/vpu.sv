`default_nettype none
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/14/2024 04:42:26 AM
// Design Name: 
// Module Name: vpu
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module vpu #(
    parameter SCREEN_W = 320,
    parameter SCREEN_H = 240,
    parameter SCREEN_HBLANK = 80,
    parameter SCREEN_VBLANK = 80
)(
    input wire clk,
    input wire rst_n,

    output wire tilemapram_en,
    output wire tilemapram_we,
    output reg [10:0] tilemapram_addr,
    input  reg [15:0] tilemapram_dout,

    output wire tileram_en,
    output wire tileram_we,
    output reg [14:0] tileram_addr,
    input  reg [7:0] tileram_dout,

    //output wire palram_en,
    //output reg [7:0] palram_addr,
    //input  reg [7:0] palram_dout,

    output wire dot_clk,
    output reg [31:0] color,
    output wire hsync,
    output wire vsync
);

typedef struct packed {
  logic [7:0]  flag;
  logic        palInfo_mode;
  logic [2:0]  palInfo_bank;
  logic [3:0]  palInfo_no;
  logic [15:0] tileIdx;
  logic [3:0]  tileNo;
  logic [1:0]  tileSize;
  logic [1:0]  _reserved2;
  logic [15:0] x;
  logic [15:0] y;
} Sprite;

typedef struct packed {
  logic [7:0]  flag;
  logic        palInfo_mode;
  logic [2:0]  palInfo_bank;
  logic [3:0]  palInfo_no;
  logic [15:0] tileIdx;
  logic [3:0]  tileNo;
  logic [1:0]  tileSize;
  logic [1:0]  _reserved2;
  logic [15:0] x;
  logic [15:0] y;
} BG;

localparam SCREEN_X_DOTS = SCREEN_W + SCREEN_HBLANK;
localparam SCREEN_Y_DOTS = SCREEN_H + SCREEN_VBLANK;

BG bg[4];
Sprite sp[4];

logic [31:0] scanline_buffer[2][320];

logic [8:0] x;
logic [8:0] y;

assign hsync = x < SCREEN_W;
assign vsync = y < SCREEN_H;

// update x, y
always_ff @(posedge clk) begin
    if (~rst_n) begin
        x <= 0;
        y <= 0;
    end
    else begin
        if (x != SCREEN_X_DOTS - 1) begin
            x <= x + 1;
        end else begin
            x <= 0;
            if (y != SCREEN_Y_DOTS - 1) begin
                y <= y + 1;
            end else begin
                y <= 0;
            end
        end
    end
end

// ================================================================
// get sprite/bg information and store FIFO (memory access (read))
always_ff @(posedge clk) begin
    if (~rst_n) begin
      sp[0].flag <= 0;
      sp[0].palInfo_mode <= 0;  // 0:tile mode, 1:pixel mode
      sp[0].palInfo_bank <= 0;
      sp[0].palInfo_no <= 0;
      sp[0].tileIdx <= 0;
      sp[0].tileNo <= 0;
      sp[0].tileSize <= 1;  // 0:8x8, 1:16x16, 2:32x32, 3:64x64
      sp[0]._reserved2 <= 0;
      sp[0].x <= 0;
      sp[0].y <= 0;

      bg[0].flag <= 0;
      bg[0].palInfo_mode <= 0;  // 0:tile mode, 1:pixel mode
      bg[0].palInfo_bank <= 0;
      bg[0].palInfo_no <= 0;
      bg[0].tileIdx <= 0;
      bg[0].tileNo <= 0;
      bg[0].tileSize <= 1;  // 0:8x8, 1:16x16, 2:32x32, 3:64x64
      bg[0]._reserved2 <= 0;
      bg[0].x <= 0;
      bg[0].y <= 0;
    end
end
logic prev_vsync = 0;
always_ff @(posedge clk) begin
    prev_vsync <= vsync;
    if (!vsync && prev_vsync) begin
      bg[0].x <= bg[0].x + 1;
      bg[0].y <= bg[0].y + 1;
    end
end

// ================================================================
// cumpute {x0, y0} : affine transformed {x_, y_}
function logic [7:0] spRealTileSizeX(input [2:0] tileSize);
   case(tileSize)
     2'b00 : spRealTileSizeX = 16'd8;
     2'b01 : spRealTileSizeX = 16'd16;
     2'b10 : spRealTileSizeX = 16'd32;
     2'b11 : spRealTileSizeX = 16'd64;
   endcase
endfunction
function logic [7:0] spRealTileSizeY(input [2:0] tileSize);
   case(tileSize)
     2'b00 : spRealTileSizeY = 16'd8;
     2'b01 : spRealTileSizeY = 16'd16;
     2'b10 : spRealTileSizeY = 16'd32;
     2'b11 : spRealTileSizeY = 16'd64;
   endcase
endfunction
logic [15:0] beginX;
logic [15:0] beginY;
logic [15:0] endX;
logic [15:0] endY;
shortint x0;
shortint y0;
logic isin_rect_area_y;
logic isin_draw_area_x;
logic isin_draw_area_y;
logic isin_draw_area;
assign isin_rect_area_y = rst_n && (beginY <= y && y < endY);
always_comb begin
  //beginX = sp[0].x;
  //beginY = sp[0].y;
  //endX = sp[0].x + spRealTileSizeX(sp[0].tileSize);
  //endY = sp[0].y + spRealTileSizeY(sp[0].tileSize);
  beginX = 0;
  beginY = 0;
  endX = SCREEN_W;
  endY = SCREEN_H;
  // judge area
  if (isin_rect_area_y) begin
    //isin_draw_area_x = hsync && x0 >= 0 && x0 < spRealTileSizeX(sp[0].tileSize);
    //isin_draw_area_y = vsync && y0 >= 0 && y0 < spRealTileSizeY(sp[0].tileSize);
    isin_draw_area_x = hsync;
    isin_draw_area_y = vsync;
    isin_draw_area = isin_rect_area_y && isin_draw_area_x && isin_draw_area_y;
  end
end
always_ff @(posedge clk) begin
  if (rst_n) begin
    x0 <= x - bg[0].x;
    y0 <= y - bg[0].y;
  end
end

// ================================================================
// get tileIdx from tilemap
// compute tileIdx and tileOffset
logic tilemapram_isin_draw_area;
shortint tx0, ty0;
shortint tw, th;
shortint toffset0;
logic [15:0] tilemapIdx;
logic [15:0] tileIdxOffset;
logic [15:0] tileIdx;
shortint tileX;
shortint tileY;

always_comb begin
  if (isin_draw_area) begin
    tx0 = x0 % 512;
    ty0 = y0 % 512;
    // tileIdxOffset = (x0 / 8) + (y0 / 8) * (spRealTileSizeX(sp[0].tileSize) / 8);  // BG:memory access (read), SP:not access
    // tileIdx = sp[0].tileIdx + tileIdxOffset;
    tw = spRealTileSizeX(bg[0].tileSize);
    th = spRealTileSizeY(bg[0].tileSize);
    toffset0 = ((tx0 / tw) + (ty0 / th) * (512 / tw));
  end
end

assign tilemapram_en = 1;
assign tilemapram_we = 0;

always_ff @(posedge clk) begin
  tilemapram_isin_draw_area <= isin_draw_area;
  if (tilemapram_isin_draw_area) begin
    tilemapram_addr <= toffset0;
  end else begin
    tilemapram_addr <= 'h00000000;  // first pixel in tile0
  end
end

//assign tileIdx = tilemapram_isin_draw_area ? tilemapram_dout : 'h0000;
assign tilemapIdx = tilemapram_dout;

// ================================================================
// get paletteIdx from tile (memory access)
logic tileram_isin_draw_area;
shortint toffset1x, toffset1y;
logic [8:0] palIdx;

always_comb begin
  if (tilemapram_isin_draw_area) begin
    tileX = tx0 % 8;
    tileY = ty0 % 8;
    toffset1x = tx0 % tw / 8;
    toffset1y = ty0 % th / 8 * (tw / 8);
    tileIdx = tilemapIdx * ((tw / 8) * (th / 8)) + toffset1x + toffset1y;
  end
end

assign tileram_en = 1;
assign tileram_we = 0;

always_ff @(posedge clk) begin
  tileram_isin_draw_area <= tilemapram_isin_draw_area;
  if (tileram_isin_draw_area) begin
    tileram_addr <= tileIdx * 8*8 + tileY * 8 + tileX;
  end else begin
    tileram_addr <= 'h00000000;  // first pixel in tile0
  end
end

//assign palIdx = tileram_isin_draw_area ? tileram_dout : 'h00;
assign palIdx = tileram_dout;

// ================================================================
// get color from palette (memory access)
logic palram_wea;
logic [7:0] palram_addra;
logic [31:0] palram_dina, palram_douta;
logic palram_enb;
logic palram_web;
logic [7:0] palram_addrb;
logic [31:0] palram_dinb, palram_doutb;

logic palram_isin_draw_area;

assign palram_wea = 0;    // dummy
assign palram_addra = 0;  // dummy
assign palram_dina = 0;   // dummy
assign palram_douta = 0;  // dummy
assign palram_enb = 1;
assign palram_web = 0;
assign palram_dinb = 0;

palette_ram palram0 (
  .clka(clk),    // input wire clka
  .wea(palram_wea),      // input wire [0 : 0] wea
  .addra(palram_addra),  // input wire [7 : 0] addra
  .dina(palram_dina),    // input wire [31 : 0] dina
  .douta(palram_douta),  // output wire [31 : 0] douta
  .clkb(clk),    // input wire clkb
  .enb(palram_enb),      // input wire enb
  .web(palram_web),      // input wire [0 : 0] web
  .addrb(palram_addrb),  // input wire [7 : 0] addrb
  .dinb(palram_dinb),    // input wire [31 : 0] dinb
  .doutb(palram_doutb)  // output wire [31 : 0] doutb
);

always_ff @(posedge clk) begin
  // palram_isin_draw_area <= isin_draw_area;
  palram_isin_draw_area <= tileram_isin_draw_area;
  if (palram_isin_draw_area) begin
    palram_addrb <= bg[0].palInfo_no * 16 + palIdx;
  end else begin
    palram_addrb <= 'h00000000;  // clear color
  end
end

assign color = palram_isin_draw_area ? palram_doutb : 'h00000000;

endmodule
