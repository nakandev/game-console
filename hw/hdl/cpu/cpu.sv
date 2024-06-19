module cpu
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire        vsync,  // [DEBUG]

  output wire        mem_en,
  output wire        mem_we,
  output wire [31:0] mem_addr,
  output wire [31:0] mem_din,  // write to mem
  input  wire [31:0] mem_dout  // read from mem
);

localparam INIT_BEGIN    = 0;
localparam INIT_PARAM_SP = 1;
localparam INIT_PARAM_BG = 2;
localparam INIT_MAP      = 3;
localparam INIT_TILE_SP  = 4;
localparam INIT_TILE_BG  = 5;
localparam INIT_PAL_SP   = 6;
localparam INIT_PAL_BG   = 7;
localparam INIT_FIN      = 8;
int init_state = INIT_BEGIN;
int init_count = 0;
reg [31:0] init_addr;
reg [31:0] init_data;
reg        init_en;
reg        init_we;

reg [8:0] bg_x;
reg [7:0] bg_y;
reg prev_vsync;

logic [31:0] palcolor[7] = {
  32'hFF000000/*Black*/,
  32'hFF0000FF/*Red*/,
  32'hFF00FF00/*Blue*/,
  32'hFFFF0000/*Green*/,
  32'hFF00FFFF/*Yellow*/,
  32'hFFFF00FF/*Purple*/,
  32'hFFFFFFFF/*While*/
  // 32'hFF7F7F7F/*Gray*/
};

assign mem_addr = init_addr;
assign mem_din = init_data;
assign mem_en = init_en;
assign mem_we = init_we;

always_ff @(posedge clk) begin
  if (~rst_n) begin
    bg_x <= 0;
    bg_y <= 0;
    prev_vsync <= 0;
  end
  else begin
    if ((init_state == INIT_BEGIN    && init_count == 100-1)
      ||(init_state == INIT_PARAM_SP && init_count == 128*5-1)
      ||(init_state == INIT_PARAM_BG && init_count == 4*5-1)
      ||(init_state == INIT_MAP      && init_count == 2048-1)
      ||(init_state == INIT_TILE_SP  && init_count == 256*64-1)
      ||(init_state == INIT_TILE_BG  && init_count == 256*64-1)
      ||(init_state == INIT_PAL_SP   && init_count == 256-1)
      ||(init_state == INIT_PAL_BG   && init_count == 256-1)
    ) begin
      init_count <= 0;
      init_state <= init_state + 1;
    end else if (init_state < INIT_FIN) begin
      init_count <= init_count + 1;
    end else begin
      if (init_count == (320+80)*(240+80)*4-1) begin
        init_count <= 0;
        bg_x <= bg_x + 1;
        bg_y <= bg_y + 1;
      end
      else begin
        init_count <= init_count + 1;
      end
    end

    if (init_state == INIT_PARAM_SP) begin
      // param sp
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0600_0000 + init_count;
      if (init_count inside {0, 5, 10, 15, 20, 25, 30, 35}) begin
        init_data <= 0 
          | (1'b1 << 31)          // sp_enable=1
          | (2'b00 << 24)         // sp_tilesize
          | ((20*(init_count/5)+20) << 8)  // sp_x
          | (( 1*(init_count/5)+20) << 0); // sp_y
      end
      else if (init_count inside {1, 6, 11, 16, 21, 26, 31, 36}) begin
        init_data <= 0
          | (4'h1 << 28)          // tile_bank=1
          | (2'h1 << 4);          // pal_bank=1
      end
      else if (init_count inside {40, 45}) begin
        init_data <= 0 
          | (1'b1 << 31)          // sp_enable=1
          | (1'b1 << 30)          // sp_affineEnable=1
          | (2'b11 << 24)         // sp_tilesize
          | ((20*(init_count/5-8)+100) << 8)  // sp_x
          | (( 1*(init_count/5-8)+100) << 0); // sp_y
      end
      else if (init_count inside {41, 46}) begin
        init_data <= 0
          | (4'h1 << 28)          // tile_bank=1
          | (2'h1 << 4);          // pal_bank=1
      end
      else if (init_count inside {42, 47}) begin
        init_data <= 0
          | (( 181 & 16'hffff) << 16)  // Ba
          | ((-181 & 16'hffff) << 0);  // Bb
      end
      else if (init_count inside {43, 48}) begin
        init_data <= 0
          | (( 181 & 16'hffff) << 16)  // Bc
          | (( 181 & 16'hffff) << 0);  // Bd
      end
      else if (init_count inside {44, 49}) begin
        init_data <= 0
          | ((  32 & 16'hffff) << 16)  // Bx
          | ((  32 & 16'hffff) << 0);  // By
      end
      else if (init_count inside {124*5, 125*5, 126*5, 127*5}) begin
        init_data <= 0 
          | (1'b1 << 31)          // sp_enable=1
          | (2'b11 << 24)         // sp_tilesize
          | ((20*(init_count/5-124)+200) << 8)  // sp_x
          | ((-3*(init_count/5-124)+200) << 0); // sp_y
      end
      else if (init_count inside {124*5+1, 125*5+1, 126*5+1, 127*5+1}) begin
        init_data <= 0
          | (4'h1 << 28)          // tile_bank=1
          | (2'h1 << 4);          // pal_bank=1
      end
      else begin
        init_data <= 0;
      end
    end
    else if (init_state == INIT_PARAM_BG) begin
      // param bg
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0600_0000 + 128*5 + init_count;
      if (init_count inside {0, 5, 10, 15}) begin
        init_data <= (1'b1 << 31);  // bg_enable=1
      end
      else begin
        init_data <= 0;
      end
    end
    else if (init_state == INIT_MAP) begin
      // map
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0610_0000 + init_count;
      init_data <= init_count % 256;
    end
    else if (init_state == INIT_TILE_SP) begin
      // tile
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0620_0000 + init_count;
      init_data <= init_count / 64;
    end
    else if (init_state == INIT_TILE_BG) begin
      // tile
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0621_0000 + init_count;
      init_data <= init_count / 64;
    end
    else if (init_state == INIT_PAL_SP) begin
      // pal
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0630_0100 + init_count;
      // init_data <= palcolor[init_count % $size(palcolor)];
      init_data <= palcolor[init_count % 7];
    end
    else if (init_state == INIT_PAL_BG) begin
      // pal
      init_en <= 1;
      init_we <= 1;
      init_addr <= 32'h0630_0000 + init_count;
      init_data <= palcolor[init_count % 7];
    end
    else begin
      // init_en <= 1;
      init_en <= (vsync && !prev_vsync);
      init_we <= 1;
      init_addr <= 32'h0600_0000 + 128*5 + 3*5 + 0;  // BG3 param[0]
      init_data <= (1'b1 << 31) | (bg_x << 8) | (bg_y);
      // init_en <= 0;
      // init_we <= 0;
      // init_addr <= 32'h0000_0000;
      // init_data <= 0;
    end
    prev_vsync <= vsync;
  end
end

endmodule

