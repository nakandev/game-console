module vram
  import gameconsole_pkg::*;
(
  input  wire        clk,
  input  wire        rst_n,

  input  wire        mem_en,
  input  wire        mem_we,
  input  wire [31:0] mem_addr,
  input  wire [31:0] mem_din,
  output wire [31:0] mem_dout,

  input  wire                   bg_param_ram_enb    ,
  input  wire [SP_ADDR_W-1:0]   bg_param_ram_addrb  ,
  input  wire [SP_DATA_W-1:0]   bg_param_ram_dinb   ,
  output reg  [SP_DATA_W-1:0]   bg_param_ram_doutb  ,

  input  wire                   bg_map_ram_enb   ,
  input  wire [MAP_ADDR_W-1:0]  bg_map_ram_addrb ,
  input  wire [MAP_DATA_W-1:0]  bg_map_ram_dinb  ,
  output reg  [MAP_DATA_W-1:0]  bg_map_ram_doutb ,

  input  wire                   bg_tile_ram_enb  ,
  input  wire [TILE_ADDR_W-1:0] bg_tile_ram_addrb,
  input  wire [TILE_DATA_W-1:0] bg_tile_ram_dinb ,
  output reg  [TILE_DATA_W-1:0] bg_tile_ram_doutb,

  input  wire                   bg_pal_ram_enb   ,
  input  wire [PAL_ADDR_W-1:0]  bg_pal_ram_addrb ,
  input  wire [PAL_DATA_W-1:0]  bg_pal_ram_dinb  ,
  output reg  [PAL_DATA_W-1:0]  bg_pal_ram_doutb ,

  input  wire                   sp_param_ram_enb    ,
  input  wire [SP_ADDR_W-1:0]   sp_param_ram_addrb  ,
  input  wire [SP_DATA_W-1:0]   sp_param_ram_dinb   ,
  output reg  [SP_DATA_W-1:0]   sp_param_ram_doutb  ,

  input  wire                   sp_tile_ram_enb  ,
  input  wire [TILE_ADDR_W-1:0] sp_tile_ram_addrb,
  input  wire [TILE_DATA_W-1:0] sp_tile_ram_dinb ,
  output reg  [TILE_DATA_W-1:0] sp_tile_ram_doutb,

  input  wire                   sp_pal_ram_enb   ,
  input  wire [PAL_ADDR_W-1:0]  sp_pal_ram_addrb ,
  input  wire [PAL_DATA_W-1:0]  sp_pal_ram_dinb  ,
  output reg  [PAL_DATA_W-1:0]  sp_pal_ram_doutb
);

wire                   param_ram_ena    ;
wire [SP_ADDR_W-1:0]   param_ram_addra  ;
wire [SP_DATA_W-1:0]   param_ram_dina   ;
reg  [SP_DATA_W-1:0]   param_ram_douta  ;
wire                   param_ram_enb    ;
wire [SP_ADDR_W-1:0]   param_ram_addrb  ;
wire [SP_DATA_W-1:0]   param_ram_dinb   ;
reg  [SP_DATA_W-1:0]   param_ram_doutb  ;

wire                   map_ram_ena   ;
wire [MAP_ADDR_W-1:0]  map_ram_addra ;
wire [MAP_DATA_W-1:0]  map_ram_dina  ;
reg  [MAP_DATA_W-1:0]  map_ram_douta ;
wire                   map_ram_ena_Bank  [2**MAP_BANK_W];
wire                   map_ram_wea_Bank  [2**MAP_BANK_W];
wire [MAP_INDX_W-1:0]  map_ram_addra_Bank[2**MAP_BANK_W];
wire [MAP_DATA_W-1:0]  map_ram_dina_Bank [2**MAP_BANK_W];
wire [MAP_DATA_W-1:0]  map_ram_douta_Bank[2**MAP_BANK_W];
wire                   map_ram_enb_Bank  [2**MAP_BANK_W];
wire                   map_ram_web_Bank  [2**MAP_BANK_W];
wire [MAP_INDX_W-1:0]  map_ram_addrb_Bank[2**MAP_BANK_W];
wire [MAP_DATA_W-1:0]  map_ram_dinb_Bank [2**MAP_BANK_W];
wire [MAP_DATA_W-1:0]  map_ram_doutb_Bank[2**MAP_BANK_W];

wire                   tile_ram_ena  ; 
wire [TILE_ADDR_W-1:0] tile_ram_addra; 
wire [TILE_DATA_W-1:0] tile_ram_dina ; 
reg  [TILE_DATA_W-1:0] tile_ram_douta; 
wire                   tile_ram_ena_Bank  [2**TILE_BANK_W];
wire                   tile_ram_wea_Bank  [2**TILE_BANK_W];
wire [TILE_INDX_W-1:0] tile_ram_addra_Bank[2**TILE_BANK_W];
wire [TILE_DATA_W-1:0] tile_ram_dina_Bank [2**TILE_BANK_W];
wire [TILE_DATA_W-1:0] tile_ram_douta_Bank[2**TILE_BANK_W];
wire                   tile_ram_enb_Bank  [2**TILE_BANK_W];
wire                   tile_ram_web_Bank  [2**TILE_BANK_W];
wire [TILE_INDX_W-1:0] tile_ram_addrb_Bank[2**TILE_BANK_W];
wire [TILE_DATA_W-1:0] tile_ram_dinb_Bank [2**TILE_BANK_W];
wire [TILE_DATA_W-1:0] tile_ram_doutb_Bank[2**TILE_BANK_W];

wire                   pal_ram_ena   ;
wire [PAL_ADDR_W-1:0]  pal_ram_addra ;
wire [PAL_DATA_W-1:0]  pal_ram_dina  ;
reg  [PAL_DATA_W-1:0]  pal_ram_douta ;
wire                   pal_ram_ena_Bank  [2**PAL_BANK_W];
wire                   pal_ram_wea_Bank  [2**PAL_BANK_W];
wire [PAL_INDX_W-1:0]  pal_ram_addra_Bank[2**PAL_BANK_W];
wire [PAL_DATA_W-1:0]  pal_ram_dina_Bank [2**PAL_BANK_W];
wire [PAL_DATA_W-1:0]  pal_ram_douta_Bank[2**PAL_BANK_W];
wire                   pal_ram_enb_Bank  [2**PAL_BANK_W];
wire                   pal_ram_web_Bank  [2**PAL_BANK_W];
wire [PAL_INDX_W-1:0]  pal_ram_addrb_Bank[2**PAL_BANK_W];
wire [PAL_DATA_W-1:0]  pal_ram_dinb_Bank [2**PAL_BANK_W];
wire [PAL_DATA_W-1:0]  pal_ram_doutb_Bank[2**PAL_BANK_W];

reg param_ram_web;
reg bg_map_ram_web;
reg bg_tile_ram_web;
reg sp_tile_ram_web;
reg bg_pal_ram_web;
reg sp_pal_ram_web;

assign param_ram_ena     = mem_en && mem_addr[31:16] == 16'h0600;
assign param_ram_wea     = mem_we;
assign param_ram_addra   = mem_addr[SP_ADDR_W-1:0];
assign param_ram_dina    = mem_din [SP_DATA_W-1:0];

assign map_ram_ena    = mem_en && mem_addr[31:16] == 16'h0610;
assign map_ram_wea    = mem_we;
assign map_ram_addra  = mem_addr[MAP_ADDR_W-1:0];
assign map_ram_dina   = mem_din [MAP_DATA_W-1:0];

assign tile_ram_ena   = mem_en && mem_addr[31:16] == 16'h0620;
assign tile_ram_wea   = mem_we;
assign tile_ram_addra = mem_addr[TILE_ADDR_W-1:0];
assign tile_ram_dina  = mem_din [TILE_DATA_W-1:0];

assign pal_ram_ena    = mem_en && mem_addr[31:16] == 16'h0630;
assign pal_ram_wea    = mem_we;
assign pal_ram_addra  = mem_addr[PAL_ADDR_W-1:0];
assign pal_ram_dina   = mem_din [PAL_DATA_W-1:0];

assign mem_dout = 
 (mem_addr[31:16] == 16'h0600) ? param_ram_douta   :
 (mem_addr[31:16] == 16'h0610) ? map_ram_douta  :
 (mem_addr[31:16] == 16'h0620) ? tile_ram_douta :
 (mem_addr[31:16] == 16'h0630) ? pal_ram_douta  :
                                 32'h00000000;

assign param_ram_web   = 1'b0;
assign bg_map_ram_web  = 1'b0;
assign bg_tile_ram_web = 1'b0;
assign sp_tile_ram_web = 1'b0;
assign bg_pal_ram_web  = 1'b0;
assign sp_pal_ram_web  = 1'b0;

genvar i;

// [TODO] fix select {gb, sp};
assign param_ram_enb   = bg_param_ram_enb | sp_param_ram_enb;
assign param_ram_addrb = bg_param_ram_enb ? bg_param_ram_addrb[BG_ADDR_W-1:0] :
                         sp_param_ram_enb ? sp_param_ram_addrb[SP_ADDR_W-1:0] : 0;
assign param_ram_dinb  = bg_param_ram_enb ? bg_param_ram_dinb :
                         sp_param_ram_enb ? sp_param_ram_dinb : 0;
always_comb begin
  bg_param_ram_doutb = param_ram_doutb;
  sp_param_ram_doutb = param_ram_doutb;
end

bram_tdp_rf_rf #(
  .ADDR_W(SP_ADDR_W),
  .DATA_W(SP_DATA_W)
) param_ram (
  .clka (clk),
  .ena  (param_ram_ena  ),
  .wea  (param_ram_wea  ),
  .addra(param_ram_addra),
  .dina (param_ram_dina ),
  .douta(param_ram_douta),
  .clkb (clk),
  .enb  (param_ram_enb  ),
  .web  (param_ram_web  ),
  .addrb(param_ram_addrb),
  .dinb (param_ram_dinb ),
  .doutb(param_ram_doutb)
);

assign    map_ram_addra_sel =    map_ram_addra[MAP_ADDR_W-1:MAP_INDX_W];
assign bg_map_ram_addrb_sel = bg_map_ram_addrb[MAP_ADDR_W-1:MAP_INDX_W];
always_comb begin
     map_ram_douta = map_ram_douta_Bank[   map_ram_addra_sel];
  bg_map_ram_doutb = map_ram_doutb_Bank[bg_map_ram_addrb_sel];
end

generate
  for (i=0; i < 2**MAP_BANK_W; i++) begin
    assign map_ram_ena_Bank[i]   = map_ram_ena && (map_ram_addra_sel == i);
    assign map_ram_wea_Bank[i]   = map_ram_wea && (map_ram_addra_sel == i);
    assign map_ram_addra_Bank[i] = map_ram_addra[MAP_INDX_W-1:0];
    assign map_ram_dina_Bank[i]  = map_ram_dina;
    assign map_ram_enb_Bank[i]   = bg_map_ram_enb && (bg_map_ram_addrb_sel == i);
    assign map_ram_web_Bank[i]   = bg_map_ram_web && (bg_map_ram_addrb_sel == i);
    assign map_ram_addrb_Bank[i] = bg_map_ram_addrb[MAP_INDX_W-1:0];
    assign map_ram_dinb_Bank[i]  = bg_map_ram_dinb;
    bram_tdp_rf_rf #(
      .ADDR_W(MAP_INDX_W),
      .DATA_W(MAP_DATA_W)
    ) map_ram (
      .clka (clk),
      .ena  (map_ram_ena_Bank[i]  ),
      .wea  (map_ram_wea_Bank[i]  ),
      .addra(map_ram_addra_Bank[i]),
      .dina (map_ram_dina_Bank[i] ),
      .douta(map_ram_douta_Bank[i]),
      .clkb (clk),
      .enb  (map_ram_enb_Bank[i]  ),
      .web  (map_ram_web_Bank[i]  ),
      .addrb(map_ram_addrb_Bank[i]),
      .dinb (map_ram_dinb_Bank[i] ),
      .doutb(map_ram_doutb_Bank[i])
    );
  end
endgenerate


assign    tile_ram_addra_sel =    tile_ram_addra[TILE_ADDR_W-1:TILE_INDX_W];
assign bg_tile_ram_addrb_sel = bg_tile_ram_addrb[TILE_ADDR_W-1:TILE_INDX_W];
assign sp_tile_ram_addrb_sel = sp_tile_ram_addrb[TILE_ADDR_W-1:TILE_INDX_W];
always_comb begin
     tile_ram_douta = tile_ram_douta_Bank[   tile_ram_addra_sel];
  bg_tile_ram_doutb = tile_ram_doutb_Bank[bg_tile_ram_addrb_sel];
  sp_tile_ram_doutb = tile_ram_doutb_Bank[sp_tile_ram_addrb_sel];
end

generate
  for (i=0; i < 2**TILE_BANK_W; i++) begin
    assign tile_ram_ena_Bank[i]   = (   tile_ram_addra_sel == i) ?    tile_ram_ena : 0;
    assign tile_ram_wea_Bank[i]   = (   tile_ram_addra_sel == i) ?    tile_ram_wea : 0;
    assign tile_ram_addra_Bank[i] = (   tile_ram_addra_sel == i) ?    tile_ram_addra[TILE_INDX_W-1:0] : 0;
    assign tile_ram_dina_Bank[i]  = (   tile_ram_addra_sel == i) ?    tile_ram_dina : 0;
    assign tile_ram_enb_Bank[i]   = (bg_tile_ram_addrb_sel == i) ? bg_tile_ram_enb :
                                    (sp_tile_ram_addrb_sel == i) ? sp_tile_ram_enb : 0;
    assign tile_ram_web_Bank[i]   = (bg_tile_ram_addrb_sel == i) ? bg_tile_ram_web :
                                    (sp_tile_ram_addrb_sel == i) ? sp_tile_ram_web : 0;
    assign tile_ram_addrb_Bank[i] = (bg_tile_ram_addrb_sel == i) ? bg_tile_ram_addrb[TILE_INDX_W-1:0] :
                                    (sp_tile_ram_addrb_sel == i) ? sp_tile_ram_addrb[TILE_INDX_W-1:0] : 0;
    assign tile_ram_dinb_Bank[i]  = (bg_tile_ram_addrb_sel == i) ? bg_tile_ram_dinb :
                                    (sp_tile_ram_addrb_sel == i) ? sp_tile_ram_dinb : 0;
    bram_tdp_rf_rf #(
      .ADDR_W(TILE_INDX_W),
      .DATA_W(TILE_DATA_W)
    ) tile_ram (
      .clka (clk),
      .ena  (tile_ram_ena_Bank[i]  ),
      .wea  (tile_ram_wea_Bank[i]  ),
      .addra(tile_ram_addra_Bank[i]),
      .dina (tile_ram_dina_Bank[i] ),
      .douta(tile_ram_douta_Bank[i]),
      .clkb (clk),
      .enb  (tile_ram_enb_Bank[i]  ),
      .web  (tile_ram_web_Bank[i]  ),
      .addrb(tile_ram_addrb_Bank[i]),
      .dinb (tile_ram_dinb_Bank[i] ),
      .doutb(tile_ram_doutb_Bank[i])
    );
  end
endgenerate

assign    pal_ram_addra_sel =    pal_ram_addra[PAL_ADDR_W-1:PAL_INDX_W];
assign bg_pal_ram_addrb_sel = bg_pal_ram_addrb[PAL_ADDR_W-1:PAL_INDX_W];
assign sp_pal_ram_addrb_sel = sp_pal_ram_addrb[PAL_ADDR_W-1:PAL_INDX_W];
always_comb begin
     pal_ram_douta = pal_ram_douta_Bank[   pal_ram_addra_sel];
  bg_pal_ram_doutb = pal_ram_doutb_Bank[bg_pal_ram_addrb_sel];
  sp_pal_ram_doutb = pal_ram_doutb_Bank[sp_pal_ram_addrb_sel];
end

generate
  for (i=0; i < 2**PAL_BANK_W; i++) begin
    assign pal_ram_ena_Bank[i]   = (   pal_ram_addra_sel == i) ?    pal_ram_ena : 0;
    assign pal_ram_wea_Bank[i]   = (   pal_ram_addra_sel == i) ?    pal_ram_wea : 0;
    assign pal_ram_addra_Bank[i] = (   pal_ram_addra_sel == i) ?    pal_ram_addra[PAL_INDX_W-1:0] : 0;
    assign pal_ram_dina_Bank[i]  = (   pal_ram_addra_sel == i) ?    pal_ram_dina : 0;
    assign pal_ram_enb_Bank[i]   = (bg_pal_ram_addrb_sel == i) ? bg_pal_ram_enb :
                                   (sp_pal_ram_addrb_sel == i) ? sp_pal_ram_enb : 0;
    assign pal_ram_web_Bank[i]   = (bg_pal_ram_addrb_sel == i) ? bg_pal_ram_web :
                                   (sp_pal_ram_addrb_sel == i) ? sp_pal_ram_web : 0;
    assign pal_ram_addrb_Bank[i] = (bg_pal_ram_addrb_sel == i) ? bg_pal_ram_addrb[PAL_INDX_W-1:0] :
                                   (sp_pal_ram_addrb_sel == i) ? sp_pal_ram_addrb[PAL_INDX_W-1:0] : 0;
    assign pal_ram_dinb_Bank[i]  = (bg_pal_ram_addrb_sel == i) ? bg_pal_ram_dinb :
                                   (sp_pal_ram_addrb_sel == i) ? sp_pal_ram_dinb : 0;
    bram_tdp_rf_rf #(
      .ADDR_W(PAL_INDX_W),
      .DATA_W(PAL_DATA_W)
    ) pal_ram_i (
      .clka (clk),
      .ena  (pal_ram_ena_Bank[i]  ),
      .wea  (pal_ram_wea_Bank[i]  ),
      .addra(pal_ram_addra_Bank[i]),
      .dina (pal_ram_dina_Bank[i] ),
      .douta(pal_ram_douta_Bank[i]),
      .clkb (clk),
      .enb  (pal_ram_enb_Bank[i]  ),
      .web  (pal_ram_web_Bank[i]  ),
      .addrb(pal_ram_addrb_Bank[i]),
      .dinb (pal_ram_dinb_Bank[i] ),
      .doutb(pal_ram_doutb_Bank[i])
    );
  end
endgenerate

endmodule
