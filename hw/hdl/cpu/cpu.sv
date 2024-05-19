module cpu
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  output wire        mem_en,
  output wire        mem_we,
  output wire [31:0] mem_addr,
  output wire [31:0] mem_din,  // write to mem
  input  wire [31:0] mem_dout  // read from mem
);

int init_state = 0;
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
  //32'hFF7F7F7F/*Gray*/
};

always_ff @(posedge clk) begin
  if ((init_state == 0 && init_count == 100-1)
    ||(init_state == 1 && init_count == 1024-1)
    ||(init_state == 2 && init_count == 2048-1)
    ||(init_state == 3 && init_count == 64*256-1)
    ||(init_state == 4 && init_count == 256-1)
  ) begin
    init_count <= 0;
    init_state <= init_state + 1;
  end else if (init_state == 5) begin
    init_count <= 0;
  end else begin
    init_count <= init_count + 1;
  end

  if (init_state == 1) begin
    // bg/sp
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0600_0000 + init_count;
    init_data <= 0;
  end else
  if (init_state == 2) begin
    // map
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0610_0000 + init_count;
    init_data <= init_count % 256;
  end else
  if (init_state == 3) begin
    // tile
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0620_0000 + init_count;
    init_data <= init_count / 64;
  end else
  if (init_state == 4) begin
    // pal
    init_en <= 1;
    init_we <= 1;
    init_addr <= 32'h0630_0000 + init_count;
    init_data <= palcolor[init_count % 7];
  end else begin
    init_en <= 0;
    init_we <= 0;
    init_addr <= 32'h0000_0000;
    init_data <= 0;
  end
end

endmodule

