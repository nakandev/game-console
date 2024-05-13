module vpu
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire        mem_en,
  input  wire        mem_we,
  input  wire        mem_addr,
  input  wire        mem_din,
  output wire        mem_dout,

  output wire        dot_clk,
  output reg  [31:0] color,
  output wire        hsync,
  output wire        vsync
);


wire                   sp_ram_ena    , sp_ram_enb    ;
wire [SP_ADDR_W-1:0]   sp_ram_addra  , sp_ram_addrb  ;
wire [SP_DATA_W-1:0]   sp_ram_dina   , sp_ram_dinb   ;
wire [SP_DATA_W-1:0]   sp_ram_douta  , sp_ram_doutb  ;

wire                   map_ram_ena   , map_ram_enb   ;
wire [MAP_ADDR_W-1:0]  map_ram_addra , map_ram_addrb ;
wire [MAP_DATA_W-1:0]  map_ram_dina  , map_ram_dinb  ;
wire [MAP_DATA_W-1:0]  map_ram_douta , map_ram_doutb ;

wire                   tile_ram_ena  , tile_ram_enb  ;
wire [TILE_ADDR_W-1:0] tile_ram_addra, tile_ram_addrb;
wire [TILE_DATA_W-1:0] tile_ram_dina , tile_ram_dinb ;
wire [TILE_DATA_W-1:0] tile_ram_douta, tile_ram_doutb;

wire                   pal_ram_ena   , pal_ram_enb   ;
wire [PAL_ADDR_W-1:0]  pal_ram_addra , pal_ram_addrb ;
wire [PAL_DATA_W-1:0]  pal_ram_dina  , pal_ram_dinb  ;
wire [PAL_DATA_W-1:0]  pal_ram_douta , pal_ram_doutb ;

reg        dot_clk;
reg [31:0] color;
reg        hsync, vsync;

vpu_core (
  .clk          (clk),
  .rst_n        (rst_n),

  .sp_en    (sp_ram_enb),
  .sp_addr  (sp_ram_addrb),
  .sp_dout  (sp_ram_doutb),

  .map_en   (map_ram_enb),
  .map_addr (map_ram_addrb),
  .map_dout (map_ram_doutb),

  .tile_en  (tile_ram_enb),
  .tile_addr(tile_ram_addrb),
  .tile_dout(tile_ram_doutb),

  .pal_en   (pal_ram_enb),
  .pal_addr (pal_ram_addrb),
  .pal_dout (pal_ram_doutb),

  .dot_clk      (dot_clk),
  .color        (color),
  .hsync        (hsync),
  .vsync        (vsync)
);

bram_tdp_rf_rf #(
  .ADDR_W(SP_ADDR_W),
  .DATA_W(SP_DATA_W)
) sp_ram (
  .clka (clk),
  .ena  (sp_ram_ena  ),
  .wea  (sp_ram_wea  ),
  .addra(sp_ram_addra),
  .dina (sp_ram_dina ),
  .douta(sp_ram_douta),
  .clkb (clk),
  .enb  (sp_ram_enb  ),
  .web  (sp_ram_web  ),
  .addrb(sp_ram_addrb),
  .dinb (sp_ram_dinb ),
  .doutb(sp_ram_doutb)
);

bram_tdp_rf_rf #(
  .ADDR_W(SP_ADDR_W),
  .DATA_W(SP_DATA_W)
) map_ram (
  .clka (clk),
  .ena  (map_ram_ena  ),
  .wea  (map_ram_wea  ),
  .addra(map_ram_addra),
  .dina (map_ram_dina ),
  .douta(map_ram_douta),
  .clkb (clk),
  .enb  (map_ram_enb  ),
  .web  (map_ram_web  ),
  .addrb(map_ram_addrb),
  .dinb (map_ram_dinb ),
  .doutb(map_ram_doutb)
);

bram_tdp_rf_rf #(
  .ADDR_W(SP_ADDR_W),
  .DATA_W(SP_DATA_W)
) tile_ram (
  .clka (clk),
  .ena  (tile_ram_ena  ),
  .wea  (tile_ram_wea  ),
  .addra(tile_ram_addra),
  .dina (tile_ram_dina ),
  .douta(tile_ram_douta),
  .clkb (clk),
  .enb  (tile_ram_enb  ),
  .web  (tile_ram_web  ),
  .addrb(tile_ram_addrb),
  .dinb (tile_ram_dinb ),
  .doutb(tile_ram_doutb)
);

bram_tdp_rf_rf #(
  .ADDR_W(SP_ADDR_W),
  .DATA_W(SP_DATA_W)
) pal_ram (
  .clka (clk),
  .ena  (pal_ram_ena  ),
  .wea  (pal_ram_wea  ),
  .addra(pal_ram_addra),
  .dina (pal_ram_dina ),
  .douta(pal_ram_douta),
  .clkb (clk),
  .enb  (pal_ram_enb  ),
  .web  (pal_ram_web  ),
  .addrb(pal_ram_addrb),
  .dinb (pal_ram_dinb ),
  .doutb(pal_ram_doutb)
);

endmodule
