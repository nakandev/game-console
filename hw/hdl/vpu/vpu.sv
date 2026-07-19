module vpu
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire         cpu_param_ram_en  ,
  input  wire [ 3:0]  cpu_param_ram_we  ,
  input  wire [ 9:0]  cpu_param_ram_addr,
  input  wire [31:0]  cpu_param_ram_din ,
  output reg  [31:0]  cpu_param_ram_dout,

  input  wire         cpu_map_ram_en    ,
  input  wire [ 3:0]  cpu_map_ram_we    ,
  input  wire [10:0]  cpu_map_ram_addr  ,
  input  wire [31:0]  cpu_map_ram_din   ,
  output reg  [31:0]  cpu_map_ram_dout  ,

  input  wire         cpu_tile_ram_en   ,
  input  wire [ 3:0]  cpu_tile_ram_we   ,
  input  wire [14:0]  cpu_tile_ram_addr ,
  input  wire [31:0]  cpu_tile_ram_din  ,
  output reg  [31:0]  cpu_tile_ram_dout ,

  input  wire         cpu_pal_ram_en    ,
  input  wire [ 3:0]  cpu_pal_ram_we    ,
  input  wire [ 8:0]  cpu_pal_ram_addr  ,
  input  wire [31:0]  cpu_pal_ram_din   ,
  output reg  [31:0]  cpu_pal_ram_dout  ,

  output wire        dot_clk,
  output reg  [31:0] color,
  output reg         hdraw,
  output reg         vdraw
);

wire                   bg_param_ram_en  , sp_param_ram_en  ;
wire [           3:0]  bg_param_ram_we  , sp_param_ram_we  ;
wire [PRM_ADDR_W-1:0]  bg_param_ram_addr, sp_param_ram_addr;
wire [PRM_DATA_W-1:0]  bg_param_ram_din , sp_param_ram_din ;
reg  [PRM_DATA_W-1:0]  bg_param_ram_dout, sp_param_ram_dout;

wire                   bg_map_ram_en    ;
wire [           3:0]  bg_map_ram_we    ;
wire [MAP_ADDR_W-1:0]  bg_map_ram_addr  ;
wire [MAP_DATA_W-1:0]  bg_map_ram_din   ;
reg  [MAP_DATA_W-1:0]  bg_map_ram_dout  ;

wire                   bg_tile_ram_en   , sp_tile_ram_en   ;
wire [            3:0] bg_tile_ram_we   , sp_tile_ram_we   ;
reg  [TILE_ADDR_W-1:0] bg_tile_ram_addr , sp_tile_ram_addr ;
wire [TILE_DATA_W-1:0] bg_tile_ram_din  , sp_tile_ram_din  ;
reg  [TILE_DATA_W-1:0] bg_tile_ram_dout , sp_tile_ram_dout ;

wire                   bg_pal_ram_en    , sp_pal_ram_en    ;
wire [            3:0] bg_pal_ram_we    , sp_pal_ram_we    ;
wire [PAL_ADDR_W-1:0]  bg_pal_ram_addr  , sp_pal_ram_addr  ;
wire [PAL_DATA_W-1:0]  bg_pal_ram_din   , sp_pal_ram_din   ;
reg  [PAL_DATA_W-1:0]  bg_pal_ram_dout  , sp_pal_ram_dout  ;

wire [31:0] sp_linebuffer[320];

assign bg_param_ram_din = 0;
assign bg_map_ram_din   = 0;
assign bg_tile_ram_din  = 0;
assign bg_pal_ram_din   = 0;
assign sp_param_ram_din = 0;
assign sp_tile_ram_din  = 0;
assign sp_pal_ram_din   = 0;

assign bg_param_ram_we = 4'h0;
assign bg_map_ram_we   = 4'h0;
assign bg_tile_ram_we  = 4'h0;
assign bg_pal_ram_we   = 4'h0;

vpu_core vpu_core
(
  .clk            (clk),
  .rst_n          (rst_n),

  .bg_param_en    (bg_param_ram_en  ),
  .bg_param_addr  (bg_param_ram_addr),
  .bg_param_dout  (bg_param_ram_dout),

  .bg_map_en      (bg_map_ram_en    ),
  .bg_map_addr    (bg_map_ram_addr  ),
  .bg_map_dout    (bg_map_ram_dout  ),

  .bg_tile_en     (bg_tile_ram_en   ),
  .bg_tile_addr   (bg_tile_ram_addr ),
  .bg_tile_dout   (bg_tile_ram_dout ),

  .bg_pal_en      (bg_pal_ram_en    ),
  .bg_pal_addr    (bg_pal_ram_addr  ),
  .bg_pal_dout    (bg_pal_ram_dout  ),

  .sp_param_en    (sp_param_ram_en  ),
  .sp_param_addr  (sp_param_ram_addr),
  .sp_param_dout  (sp_param_ram_dout),

  .sp_tile_en     (sp_tile_ram_en   ),
  .sp_tile_addr   (sp_tile_ram_addr ),
  .sp_tile_dout   (sp_tile_ram_dout ),

  .sp_pal_en      (sp_pal_ram_en    ),
  .sp_pal_addr    (sp_pal_ram_addr  ),
  .sp_pal_dout    (sp_pal_ram_dout  ),

  .dot_clk        (dot_clk),
  .color          (color),
  .hdraw          (hdraw),
  .vdraw          (vdraw)
);


wire        cpu_sp_param_ram_en  ;
wire [ 3:0] cpu_sp_param_ram_we  ;
wire [ 9:0] cpu_sp_param_ram_addr;
wire [31:0] cpu_sp_param_ram_din ;
wire [31:0] cpu_sp_param_ram_dout;

wire        cpu_sp_map_ram_en    ;
wire [ 3:0] cpu_sp_map_ram_we    ;
wire [10:0] cpu_sp_map_ram_addr  ;
wire [31:0] cpu_sp_map_ram_din   ;
wire [31:0] cpu_sp_map_ram_dout  ;

wire        cpu_sp_tile_ram_en   ;
wire [ 3:0] cpu_sp_tile_ram_we   ;
wire [13:0] cpu_sp_tile_ram_addr ;
wire [31:0] cpu_sp_tile_ram_din  ;
wire [31:0] cpu_sp_tile_ram_dout ;

wire        cpu_sp_pal_ram_en    ;
wire [ 3:0] cpu_sp_pal_ram_we    ;
wire [ 8:0] cpu_sp_pal_ram_addr  ;
wire [31:0] cpu_sp_pal_ram_din   ;
wire [31:0] cpu_sp_pal_ram_dout  ;

assign cpu_sp_param_ram_en   = !(hdraw && vdraw) ? cpu_param_ram_en   : sp_param_ram_en  ;
assign cpu_sp_param_ram_we   = !(hdraw && vdraw) ? cpu_param_ram_we   : sp_param_ram_we  ;
assign cpu_sp_param_ram_addr = !(hdraw && vdraw) ? cpu_param_ram_addr : sp_param_ram_addr;
assign cpu_sp_param_ram_din  = !(hdraw && vdraw) ? cpu_param_ram_din  : sp_param_ram_din ;
always_comb begin
  cpu_param_ram_dout = cpu_sp_param_ram_dout;
   sp_param_ram_dout = cpu_sp_param_ram_dout;
end

assign cpu_sp_map_ram_en     = !(hdraw && vdraw) ? cpu_map_ram_en   :  1'b0;
assign cpu_sp_map_ram_we     = !(hdraw && vdraw) ? cpu_map_ram_we   :  4'b0;
assign cpu_sp_map_ram_addr   = !(hdraw && vdraw) ? cpu_map_ram_addr : 11'b0;
assign cpu_sp_map_ram_din    = !(hdraw && vdraw) ? cpu_map_ram_din  : 32'b0;
always_comb begin
  cpu_map_ram_dout = cpu_sp_map_ram_dout;
end

assign cpu_sp_tile_ram_en    = !(hdraw && vdraw) ? cpu_tile_ram_en   : sp_tile_ram_en;
assign cpu_sp_tile_ram_we    = !(hdraw && vdraw) ? cpu_tile_ram_we   : sp_tile_ram_we;
assign cpu_sp_tile_ram_addr  = !(hdraw && vdraw) ? cpu_tile_ram_addr : sp_tile_ram_addr[TILE_ADDR_W-1:2];
assign cpu_sp_tile_ram_din   = !(hdraw && vdraw) ? cpu_tile_ram_din  : sp_tile_ram_din;
reg [TILE_ADDR_W-1:0] sp_tile_ram_addr_delay1;
always_ff @(posedge clk) begin
  sp_tile_ram_addr_delay1 <= sp_tile_ram_addr;
end
always_comb begin
  cpu_tile_ram_dout = cpu_sp_tile_ram_dout;
  case (sp_tile_ram_addr_delay1[1:0])
    2'd0: sp_tile_ram_dout = cpu_sp_tile_ram_dout[ 7: 0];
    2'd1: sp_tile_ram_dout = cpu_sp_tile_ram_dout[15: 8];
    2'd2: sp_tile_ram_dout = cpu_sp_tile_ram_dout[23:16];
    2'd3: sp_tile_ram_dout = cpu_sp_tile_ram_dout[31:24];
    default: sp_tile_ram_dout = 8'h00;
  endcase
end

assign cpu_sp_pal_ram_en     = !(hdraw && vdraw) ? cpu_pal_ram_en   : sp_pal_ram_en  ;
assign cpu_sp_pal_ram_we     = !(hdraw && vdraw) ? cpu_pal_ram_we   : sp_pal_ram_we  ;
assign cpu_sp_pal_ram_addr   = !(hdraw && vdraw) ? cpu_pal_ram_addr : sp_pal_ram_addr;
assign cpu_sp_pal_ram_din    = !(hdraw && vdraw) ? cpu_pal_ram_din  : sp_pal_ram_din ;
always_comb begin
  cpu_pal_ram_dout = cpu_sp_pal_ram_dout;
   sp_pal_ram_dout = cpu_sp_pal_ram_dout;
end

wire [ 9:0]BRAM_PORTB_0_addr;
wire [31:0]BRAM_PORTB_0_din;
wire [31:0]BRAM_PORTB_0_dout;
wire       BRAM_PORTB_0_en;
wire [ 3:0]BRAM_PORTB_0_we;
wire [11:0]BRAM_PORTB_1_addr;
wire [15:0]BRAM_PORTB_1_din;
wire [15:0]BRAM_PORTB_1_dout;
wire       BRAM_PORTB_1_en;
wire [ 1:0]BRAM_PORTB_1_we;
wire [15:0]BRAM_PORTB_2_addr;
wire [ 7:0]BRAM_PORTB_2_din;
wire [ 7:0]BRAM_PORTB_2_dout;
wire       BRAM_PORTB_2_en;
wire [ 0:0]BRAM_PORTB_2_we;
wire [ 8:0]BRAM_PORTB_3_addr;
wire [31:0]BRAM_PORTB_3_din;
wire [31:0]BRAM_PORTB_3_dout;
wire       BRAM_PORTB_3_en;
wire [ 3:0]BRAM_PORTB_3_we;

vpu_ram_wrapper vpu_ram_wrapper_0
(
  .BRAM_PORTA_0_addr (cpu_sp_param_ram_addr),
  .BRAM_PORTA_0_clk  (clk ),
  .BRAM_PORTA_0_din  (cpu_sp_param_ram_din ),
  .BRAM_PORTA_0_dout (cpu_sp_param_ram_dout),
  .BRAM_PORTA_0_en   (cpu_sp_param_ram_en  ),
  .BRAM_PORTA_0_we   (cpu_sp_param_ram_we  ),

  .BRAM_PORTA_1_addr (cpu_sp_map_ram_addr),
  .BRAM_PORTA_1_clk  (clk ),
  .BRAM_PORTA_1_din  (cpu_sp_map_ram_din ),
  .BRAM_PORTA_1_dout (cpu_sp_map_ram_dout),
  .BRAM_PORTA_1_en   (cpu_sp_map_ram_en  ),
  .BRAM_PORTA_1_we   (cpu_sp_map_ram_we  ),

  .BRAM_PORTA_2_addr (cpu_sp_tile_ram_addr),
  .BRAM_PORTA_2_clk  (clk ),
  .BRAM_PORTA_2_din  (cpu_sp_tile_ram_din ),
  .BRAM_PORTA_2_dout (cpu_sp_tile_ram_dout),
  .BRAM_PORTA_2_en   (cpu_sp_tile_ram_en  ),
  .BRAM_PORTA_2_we   (cpu_sp_tile_ram_we  ),

  .BRAM_PORTA_3_addr (cpu_sp_pal_ram_addr),
  .BRAM_PORTA_3_clk  (clk ),
  .BRAM_PORTA_3_din  (cpu_sp_pal_ram_din ),
  .BRAM_PORTA_3_dout (cpu_sp_pal_ram_dout),
  .BRAM_PORTA_3_en   (cpu_sp_pal_ram_en  ),
  .BRAM_PORTA_3_we   (cpu_sp_pal_ram_we  ),

  .BRAM_PORTB_0_addr (bg_param_ram_addr),
  .BRAM_PORTB_0_clk  (clk),
  .BRAM_PORTB_0_din  (bg_param_ram_din ),
  .BRAM_PORTB_0_dout (bg_param_ram_dout),
  .BRAM_PORTB_0_en   (bg_param_ram_en  ),
  .BRAM_PORTB_0_we   (bg_param_ram_we  ),

  .BRAM_PORTB_1_addr (bg_map_ram_addr),
  .BRAM_PORTB_1_clk  (clk),
  .BRAM_PORTB_1_din  (bg_map_ram_din ),
  .BRAM_PORTB_1_dout (bg_map_ram_dout),
  .BRAM_PORTB_1_en   (bg_map_ram_en  ),
  .BRAM_PORTB_1_we   (bg_map_ram_we  ),

  .BRAM_PORTB_2_addr (bg_tile_ram_addr),
  .BRAM_PORTB_2_clk  (clk),
  .BRAM_PORTB_2_din  (bg_tile_ram_din ),
  .BRAM_PORTB_2_dout (bg_tile_ram_dout),
  .BRAM_PORTB_2_en   (bg_tile_ram_en  ),
  .BRAM_PORTB_2_we   (bg_tile_ram_we  ),

  .BRAM_PORTB_3_addr (bg_pal_ram_addr),
  .BRAM_PORTB_3_clk  (clk),
  .BRAM_PORTB_3_din  (bg_pal_ram_din ),
  .BRAM_PORTB_3_dout (bg_pal_ram_dout),
  .BRAM_PORTB_3_en   (bg_pal_ram_en  ),
  .BRAM_PORTB_3_we   (bg_pal_ram_we  )
);

endmodule
