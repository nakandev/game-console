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
localparam INIT_TILE     = 4;
localparam INIT_PAL      = 5;
localparam INIT_FIN      = 6;
int init_state = INIT_BEGIN;
// int init_state = 0;
int init_count = 0;
reg [31:0] init_addr;
reg [31:0] init_data;
reg        init_en;
reg        init_we;

assign mem_addr = init_addr;
assign mem_din = init_data;
assign mem_en = init_en;
assign mem_we = init_we;

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

reg [1:0] layer;
reg [8:0] bg_x;
reg [7:0] bg_y;

always_ff @(posedge vsync) begin
  if (init_state < INIT_FIN) begin
    bg_x <= 0;
    bg_y <= 0;
  end else begin
    layer <= layer + 1;
    if (layer == 3) begin
      bg_x <= bg_x + 1;
      bg_y <= bg_y + 1;
    end
  end
end

always_ff @(posedge clk) begin
  if ((init_state == INIT_BEGIN    && init_count == 100-1)
    // ||(init_state == INIT_PARAM_SP && init_count == 128*5-1)
    ||(init_state == INIT_PARAM_SP && init_count == 2*5-1)
    ||(init_state == INIT_PARAM_BG && init_count == 4*5-1)
    ||(init_state == INIT_MAP      && init_count == 2048-1)
    ||(init_state == INIT_TILE     && init_count == 256*64-1)
    ||(init_state == INIT_PAL      && init_count == 256-1)
  ) begin
    init_count <= 0;
    init_state <= init_state + 1;
  end else if (init_state < INIT_FIN) begin
    init_count <= init_count + 1;
  end else begin
    init_count <= 0;
  end

  if (init_state == INIT_PARAM_SP) begin
    // param sp
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0600_0000 + init_count;
    init_data <= (1'b1 << 31) | ((10*(init_count/5+1)) << 8) | ((10*(init_count/5+1)) << 0);
    // case (init_count % 5)
    //   0: init_data <= (1'b1 << 31);
    //   1: init_data <= 0;
    //   2: init_data <= 0;
    //   3: init_data <= 0;
    //   4: init_data <= 0;
    //   default: init_data <= 0;
    // endcase
  end else
  if (init_state == INIT_PARAM_BG) begin
    // param bg
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0600_0000 + 128*5 + init_count;
    init_data <= (1'b1 << 31);
    // case (init_count % 5)
    //   0: init_data <= (1'b1 << 31);
    //   1: init_data <= 0;
    //   2: init_data <= 0;
    //   3: init_data <= 0;
    //   4: init_data <= 0;
    //   default: init_data <= 0;
    // endcase
  end else
  if (init_state == INIT_MAP) begin
    // map
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0610_0000 + init_count;
    init_data <= init_count % 256;
  end else
  if (init_state == INIT_TILE) begin
    // tile
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0620_0000 + init_count;
    init_data <= init_count / 64;
  end else
  if (init_state == INIT_PAL) begin
    // pal
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0630_0000 + init_count;
    init_data <= palcolor[init_count % $size(palcolor)];
  end else begin
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0600_0000 + 128*5 + 3*5 + 0;  // BG3 param[0]
    init_data <= (1'b1 << 31) | (bg_x << 8) | (bg_y);
    // init_en <= 0;
    // init_we <= 0;
    // init_addr <= 32'h0000_0000;
    // init_data <= 0;
  end
end

endmodule

