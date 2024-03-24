`default_nettype wire
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
    parameter SCREEN_HBLANK = 0,
    parameter SCREEN_VBLANK = 0
)(
    input wire clk,
    input wire rst_n,
    output dot_clk,
    output [31:0] color,
    output hsync,
    output vsync
);

typedef struct packed {
  logic [7:0]  flag;
  logic        palInfo_mode;
  logic [2:0]  palInfo_bank;
  logic [3:0]  palInfo_no;
  logic [7:0]  tileIdx;
  logic [3:0]  tileNo;
  logic [1:0]  tileSize;
  logic [1:0]  _reserved2;
  logic [15:0] x;
  logic [15:0] y;
} Sprite;

localparam SCREEN_X_DOTS = SCREEN_W + SCREEN_HBLANK;
localparam SCREEN_Y_DOTS = SCREEN_H + SCREEN_VBLANK;

byte unsigned tileData[4*8*8]; // 16x16=4*8x8
int unsigned palData[2][16*16];
Sprite sp;

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
      sp.flag <= 0;
      sp.palInfo_mode <= 0;  // 0:tile mode, 1:pixel mode
      sp.palInfo_bank <= 0;
      sp.palInfo_no <= 0;
      sp.tileIdx <= 0;
      sp.tileNo <= 0;
      sp.tileSize <= 1;  // 0:8x8, 1:16x16, 2:32x32, 3:64x64
      sp._reserved2 <= 0;
      sp.x <= 0;
      sp.y <= 0;
      palData[0][0] <= 'hff0000ff;  // rrggbbaa
      palData[0][1] <= 'h00ff00ff;
      palData[0][2] <= 'h0000ffff;
      palData[0][3] <= 'hffff00ff;
      palData[0][4] <= 'h00ffffff;
      palData[0][5] <= 'hff00ffff;
      for (int t=0; t<4; t++) begin
        for (int j=0; j<8; j++) begin
          for (int i=0; i<8; i++) begin
            tileData[(t*8*8)+(j+0)*8+(i+0)] <= t;
          end
        end
      end
    end
end


// ================================================================
// cumpute {x0, y0} : affine transformed {x_, y_}
function logic [15:0] spRealTileSizeX(input [2:0] tileSize);
   case(tileSize)
     2'b00 : spRealTileSizeX = 16'd8;
     2'b01 : spRealTileSizeX = 16'd16;
     2'b10 : spRealTileSizeX = 16'd32;
     2'b11 : spRealTileSizeX = 16'd64;
   endcase
endfunction
function logic [15:0] spRealTileSizeY(input [2:0] tileSize);
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
assign isin_rect_area_y = rst_n && (beginY <= y && y < endY);
shortint x0;
shortint y0;
logic isin_rect_area_y;
logic isin_draw_area_x;
logic isin_draw_area_y;
logic isin_draw_area;
always_comb begin
  beginX = sp.x;
  beginY = sp.y;
  endX = sp.x + spRealTileSizeX(sp.tileSize);
  endY = sp.y + spRealTileSizeY(sp.tileSize);
  // judge area
  if (isin_rect_area_y) begin
    x0 = x - sp.x;
    y0 = y - sp.y;
    isin_draw_area_x = x0 >= 0 && x0 < spRealTileSizeX(sp.tileSize);
    isin_draw_area_y = y0 >= 0 && y0 < spRealTileSizeY(sp.tileSize);
    isin_draw_area = isin_rect_area_y && isin_draw_area_x && isin_draw_area_y;
  end
end

// ================================================================
// get tileIdx from tilemap
// compute tileIdx and tileOffset
shortint tx0;
shortint ty0;
logic [7:0] tileIdxOffset;
logic [7:0] tileIdx;
shortint tileX;
shortint tileY;

always_comb begin
  if (isin_draw_area) begin
    tx0 = x0 & (512-1); // x0 % 512;
    ty0 = y0 & (512-1); // y0 % 512;
    tileIdxOffset = (x0 / 8) + (y0 / 8) * (spRealTileSizeX(sp.tileSize) / 8);  // BG:memory access (read), SP:not access
    tileIdx = sp.tileIdx + tileIdxOffset;
    tileX = tx0 & (8-1); // tx0 % 8;
    tileY = ty0 & (8-1); // ty0 % 8;
  end
end

// ================================================================
// get paletteIdx from tile (memory access)
logic [8:0] palIdx;
always_comb begin
  if (isin_draw_area) begin
    palIdx = tileData[tileIdxOffset * 8*8 + tileY * 8 + tileX];
  end
end

// ================================================================
// get color from palette (memory access)
logic [31:0] color_out;
assign color = color_out;
always_comb begin
  if (isin_draw_area) begin
    color_out = palData[sp.palInfo_bank][sp.palInfo_no * 16 + palIdx];
  end else begin
    color_out = 'h00000000;  // clear color
  end
end

endmodule
