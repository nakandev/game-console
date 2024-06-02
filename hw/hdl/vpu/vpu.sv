module vpu
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire        mem_en,
  input  wire        mem_we,
  input  wire [31:0] mem_addr,
  input  wire [31:0] mem_din,
  output wire [31:0] mem_dout,

  output wire        dot_clk,
  output reg  [31:0] color,
  output wire        hsync,
  output wire        vsync
);

wire                   bg_param_ram_ena  , bg_param_ram_enb  , sp_param_ram_enb  ;
wire [SP_ADDR_W-1:0]   bg_param_ram_addra, bg_param_ram_addrb, sp_param_ram_addrb;
wire [SP_DATA_W-1:0]   bg_param_ram_dina , bg_param_ram_dinb , sp_param_ram_dinb ;
wire [SP_DATA_W-1:0]   bg_param_ram_douta, bg_param_ram_doutb, sp_param_ram_doutb;

wire                   bg_map_ram_ena    , bg_map_ram_enb    ;
wire [MAP_ADDR_W-1:0]  bg_map_ram_addra  , bg_map_ram_addrb  ;
wire [MAP_DATA_W-1:0]  bg_map_ram_dina   , bg_map_ram_dinb   ;
wire [MAP_DATA_W-1:0]  bg_map_ram_douta  , bg_map_ram_doutb  ;

wire                   bg_tile_ram_ena   , bg_tile_ram_enb   , sp_tile_ram_enb   ;
wire [TILE_ADDR_W-1:0] bg_tile_ram_addra , bg_tile_ram_addrb , sp_tile_ram_addrb ;
wire [TILE_DATA_W-1:0] bg_tile_ram_dina  , bg_tile_ram_dinb  , sp_tile_ram_dinb  ;
wire [TILE_DATA_W-1:0] bg_tile_ram_douta , bg_tile_ram_doutb , sp_tile_ram_doutb ;

wire                   bg_pal_ram_ena    , bg_pal_ram_enb    , sp_pal_ram_enb    ;
wire [PAL_ADDR_W-1:0]  bg_pal_ram_addra  , bg_pal_ram_addrb  , sp_pal_ram_addrb  ;
wire [PAL_DATA_W-1:0]  bg_pal_ram_dina   , bg_pal_ram_dinb   , sp_pal_ram_dinb   ;
wire [PAL_DATA_W-1:0]  bg_pal_ram_douta  , bg_pal_ram_doutb  , sp_pal_ram_doutb  ;

wire [31:0] sp_linebuffer[320];

// reg        dot_clk;
// reg [31:0] color;
// reg        hsync, vsync;

assign bg_param_ram_dinb = 0;
assign bg_map_ram_dinb   = 0;
assign bg_tile_ram_dinb  = 0;
assign bg_pal_ram_dinb   = 0;
assign sp_param_ram_dinb = 0;
assign sp_tile_ram_dinb  = 0;
assign sp_pal_ram_dinb   = 0;

vpu_core vpu_core
(
  .clk            (clk),
  .rst_n          (rst_n),

  .bg_param_en    (bg_param_ram_enb  ),
  .bg_param_addr  (bg_param_ram_addrb),
  .bg_param_dout  (bg_param_ram_doutb),

  .bg_map_en      (bg_map_ram_enb    ),
  .bg_map_addr    (bg_map_ram_addrb  ),
  .bg_map_dout    (bg_map_ram_doutb  ),

  .bg_tile_en     (bg_tile_ram_enb   ),
  .bg_tile_addr   (bg_tile_ram_addrb ),
  .bg_tile_dout   (bg_tile_ram_doutb ),

  .bg_pal_en      (bg_pal_ram_enb    ),
  .bg_pal_addr    (bg_pal_ram_addrb  ),
  .bg_pal_dout    (bg_pal_ram_doutb  ),

  .sp_param_en    (sp_param_ram_enb  ),
  .sp_param_addr  (sp_param_ram_addrb),
  .sp_param_dout  (sp_param_ram_doutb),

  .sp_tile_en     (sp_tile_ram_enb   ),
  .sp_tile_addr   (sp_tile_ram_addrb ),
  .sp_tile_dout   (sp_tile_ram_doutb ),

  .sp_pal_en      (sp_pal_ram_enb    ),
  .sp_pal_addr    (sp_pal_ram_addrb  ),
  .sp_pal_dout    (sp_pal_ram_doutb  ),

  .dot_clk        (dot_clk),
  .color          (color),
  .hsync          (hsync),
  .vsync          (vsync)
);

vram vram
(
  .clk                (clk),
  .rst_n              (rst_n),

  .mem_en             (mem_en),
  .mem_we             (mem_we),
  .mem_addr           (mem_addr),
  .mem_din            (mem_din),
  .mem_dout           (mem_dout),

  .bg_param_ram_enb   (bg_param_ram_enb  ),
  .bg_param_ram_addrb (bg_param_ram_addrb),
  .bg_param_ram_dinb  (bg_param_ram_dinb ),
  .bg_param_ram_doutb (bg_param_ram_doutb),

  .bg_map_ram_enb     (bg_map_ram_enb    ),
  .bg_map_ram_addrb   (bg_map_ram_addrb  ),
  .bg_map_ram_dinb    (bg_map_ram_dinb   ),
  .bg_map_ram_doutb   (bg_map_ram_doutb  ),

  .bg_tile_ram_enb    (bg_tile_ram_enb   ),
  .bg_tile_ram_addrb  (bg_tile_ram_addrb ),
  .bg_tile_ram_dinb   (bg_tile_ram_dinb  ),
  .bg_tile_ram_doutb  (bg_tile_ram_doutb ),

  .bg_pal_ram_enb     (bg_pal_ram_enb    ),
  .bg_pal_ram_addrb   (bg_pal_ram_addrb  ),
  .bg_pal_ram_dinb    (bg_pal_ram_dinb   ),
  .bg_pal_ram_doutb   (bg_pal_ram_doutb  ),

  .sp_param_ram_enb   (sp_param_ram_enb  ),
  .sp_param_ram_addrb (sp_param_ram_addrb),
  .sp_param_ram_dinb  (sp_param_ram_dinb ),
  .sp_param_ram_doutb (sp_param_ram_doutb),

  .sp_tile_ram_enb    (sp_tile_ram_enb   ),
  .sp_tile_ram_addrb  (sp_tile_ram_addrb ),
  .sp_tile_ram_dinb   (sp_tile_ram_dinb  ),
  .sp_tile_ram_doutb  (sp_tile_ram_doutb ),

  .sp_pal_ram_enb     (sp_pal_ram_enb    ),
  .sp_pal_ram_addrb   (sp_pal_ram_addrb  ),
  .sp_pal_ram_dinb    (sp_pal_ram_dinb   ),
  .sp_pal_ram_doutb   (sp_pal_ram_doutb  )
);
endmodule
