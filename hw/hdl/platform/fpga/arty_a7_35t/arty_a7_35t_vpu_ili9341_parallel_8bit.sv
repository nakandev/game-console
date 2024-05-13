`default_nettype none
`timescale 1ns / 1ps

module arty_a7_35t_vpu_ili9341_parallel_8bit(
    input wire CLK100MHZ,
    input wire [3:0] btn,
    output logic ck_a5,  // none
    output logic ck_a4,  // LCD_RST
    output logic ck_a3,  // LCD_CS
    output logic ck_a2,  // LCD_RS
    output logic ck_a1,  // LCD_WR
    output logic ck_a0,  // LCD_RD
    output logic ck_io8,  // LCD_D0
    output logic ck_io9,  // LCD_D1
    output logic ck_io2,  // LCD_D2
    output logic ck_io3,  // LCD_D3
    output logic ck_io4,  // LCD_D4
    output logic ck_io5,  // LCD_D5
    output logic ck_io6,  // LCD_D6
    output logic ck_io7,  // LCD_D7
    output logic [3:0] led
);
  
localparam DOTCLKS = 5; //10/2;
localparam PIXCLKS = 2; //2cycle

logic clk10MHZ = 1'b0;
logic clkPixel = 1'b0; //4 clk10MHZ
logic clkTFT = 1'b0;
logic [31:0] cnt0 = DOTCLKS-1;
logic [31:0] cnt1 = PIXCLKS-1;
logic [31:0] count = 0;
logic [7:0] data8;
logic [7:0] lcd_col_r;
logic [7:0] lcd_col_g;
logic [7:0] lcd_col_b;
logic reset200;
logic reset;
logic initialized;
logic hsync_out;
logic vsync_out;
logic prev_initialized;
logic lcd_vblank;
logic lcd_write;
logic [8:0] lcd_x;
logic [7:0] lcd_y;
logic [7:0] lcd_frame;
logic [3:0] dummy_led;

assign reset200 = btn[0];
assign reset = btn[1];
assign data8 = {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8};

assign led = {hsync_out, vsync_out, initialized};

logic CLK200MHZ;
logic locked;


int init_sprite_ram_count = 0;
int init_tilemapram_count = 0;
int init_tileram_count = 0;
int init_palram_count = 0;
logic init_sprite_ram_we;
logic init_tilemapram_we;
logic init_tileram_we;
logic init_palram_we;
assign init_sprite_ram_we = init_sprite_ram_count < 1024; // >660=5*(128+4)
assign init_tilemapram_we = init_tilemapram_count < 2048;
assign init_tileram_we = init_tileram_count < 64*256;
assign init_palram_we = init_palram_count < 256;

logic [ 9:0] init_sprite_ram_addr = 0;
logic [31:0] init_sprite_ram_data = 0;
logic [10:0] init_tilemapram_addr = 0;
logic [15:0] init_tilemapram_data = 0;
logic [14:0] init_tileram_addr = 0;
logic [ 7:0] init_tileram_data = 0;
logic [ 7:0] init_palram_addr = 0;
logic [31:0] init_palram_data = 0;


logic dot_clk;
logic [31:0] color;
logic hsync_vpu;
logic vsync_vpu;
logic sprite_ram_enb, sprite_ram_web;
logic [9:0] sprite_ram_addrb;
logic [31:0] sprite_ram_dinb, sprite_ram_doutb;
logic tilemapram_enb, tilemapram_web;
logic [10:0] tilemapram_addrb;
logic [15:0] tilemapram_dinb, tilemapram_doutb;
logic tileram_enb, tileram_web;
logic [14:0] tileram_addrb;
logic [ 7:0] tileram_dinb, tileram_doutb;
logic palram_enb, palram_web;
logic [7:0] palram_addrb;
logic [31:0] palram_dinb, palram_doutb;
  
initial begin
      initialized = 1'b0;
//      hsync_out <= 1'b0;
//      vsync_out <= 1'b0;
//      prev_initialized <= 0;
//      prev_hsync_out <= 1'b0;
//      prev_vsync_out <= 1'b0;
//      lcd_vblank <= 1'b0;
      //lcd_write = 1'b1;
//      lcd_x <= 0;
//      lcd_y <= 0;
//      lcd_frame <= 0;
//      dummy_led <= 0;
end

//clk_wiz_0 clk_100MHZ_to_200MHZ
//(
//    // Clock out ports
//    .clk_out1(CLK200MHZ),     // output clk_out1
//    // Status and control signals
//    .reset(reset200), // input reset
//    .locked(locked),       // output locked
//    // Clock in ports
//    .clk_in1(CLK100MHZ)      // input clk_in1
//);

//always_ff @(posedge CLK200MHZ) begin
always_ff @(posedge CLK100MHZ) begin
    if (cnt0 > 0) begin
        cnt0 <= cnt0 - 1'b1;
    end else begin
        cnt0 <= DOTCLKS-1;
        clk10MHZ <= ~clk10MHZ;
        // led[1] <= ~led[1];
    end
end

always_ff @(posedge clk10MHZ) begin
    if (cnt1 > 0) begin
        cnt1 <= cnt1 - 1'b1;
    end else begin
        cnt1 <= PIXCLKS-1;
        clkPixel <= ~clkPixel;
    end
end

always_ff @(posedge clkPixel) begin
    if (reset) begin
    lcd_x <= 0;
    lcd_y <= 0;
    lcd_frame <= 0;
    end
    if (hsync_out || lcd_x == 320 - 1) begin
        lcd_x <= 0;
        if (vsync_out || lcd_y == 240 - 1) begin
            lcd_y <= 0;
            lcd_frame <= lcd_frame + 1;
        end else begin
            lcd_y <= lcd_y + 1;
        end
    end else begin
        lcd_x <= lcd_x + 1;
    end
    //lcd_col_r <= lcd_x;
    //lcd_col_g <= lcd_y;
    //lcd_col_b <= lcd_frame;
    count <= count + 1;
    lcd_col_b <= color[23:16];
    lcd_col_g <= color[15: 8];
    lcd_col_r <= color[ 7: 0];
end
//assign lcd_col_a = color[31:24];
//assign lcd_col_b = color[23:16];
//assign lcd_col_g = color[15: 8];
//assign lcd_col_r = color[ 7: 0];

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

always_ff @(posedge clk10MHZ) begin
  if (init_sprite_ram_count < 1024) begin
    init_sprite_ram_count <= init_sprite_ram_count + 1;
  end
  if (init_tilemapram_count < 2048) begin
    init_tilemapram_count <= init_tilemapram_count + 1;
  end
  if (init_tileram_count < 64*256) begin
    init_tileram_count <= init_tileram_count + 1;
  end
  if (init_palram_count < 256) begin
    init_palram_count <= init_palram_count + 1;
  end

  if (init_sprite_ram_we) begin
    init_sprite_ram_addr <= init_sprite_ram_count;
    //init_sprite_ram_data <= 32'h0 + ((init_sprite_ram_count / 5)*256) + ((init_sprite_ram_count % 5) + 1);
    init_sprite_ram_data <= 0;
  end else begin
    init_sprite_ram_addr <= 'h00000000;
    init_sprite_ram_data <= 0;
  end

  if (init_tilemapram_we) begin
    init_tilemapram_addr <= init_tilemapram_count;
    init_tilemapram_data <= init_tilemapram_count % 256;
  end else begin
    init_tilemapram_addr <= 'h00000000;
    init_tilemapram_data <= 0;
  end

  if (init_tileram_we) begin
    init_tileram_addr <= init_tileram_count;
    init_tileram_data <= init_tileram_count / 64;
  end else begin
    init_tileram_addr <= 'h00000000;
    init_tileram_data <= 0;
  end

  if (init_palram_we) begin
    init_palram_addr <= init_palram_count;
    init_palram_data <= palcolor[init_palram_count % 7];
    //init_palram_data <= 
    //  ((((init_palram_count>>6)&3)<<24)*'h3F) | // a
    //  ((((init_palram_count>>4)&3)<<16)*'h3F) | // b
    //  ((((init_palram_count>>2)&3)<< 8)*'h3F) | // g
    //  ((((init_palram_count>>0)&3)<< 0)*'h3F);  // r
  end else begin
    init_palram_addr <= 'h00000000;
    init_palram_data <= 0;
  end
end

assign sprite_ram_web = 0;
assign tilemapram_web = 0;
assign tileram_web = 0;
assign palram_web = 0;

logic sprite_ram_ena, sprite_ram_wea;
logic [9:0] sprite_ram_addra;
logic [31:0] sprite_ram_dina, sprite_ram_douta;

assign sprite_ram_ena = init_sprite_ram_we;    // dummy
assign sprite_ram_wea = init_sprite_ram_we;    // dummy
assign sprite_ram_addra = init_sprite_ram_addr;  // dummy
assign sprite_ram_dina = init_sprite_ram_data;   // dummy
assign sprite_ram_douta = 0;  // dummy
assign sprite_ram_dinb = 0;

//sprite_ram spriteram00 (
bram_tdp_rf_rf #(
  .ADDR_W(10),
  .DATA_W(32)
) spriteram00 (
  .clka(clk10MHZ),    // input wire clka
  .ena(sprite_ram_ena),
  .wea(sprite_ram_wea),      // input wire [0 : 0] wea
  .addra(sprite_ram_addra),  // input wire [9 : 0] addra
  .dina(sprite_ram_dina),    // input wire [31 : 0] dina
  .douta(sprite_ram_douta),  // output wire [31 : 0] douta
  .clkb(clk10MHZ),    // input wire clkb
  .enb(sprite_ram_enb),
  .web(sprite_ram_web),      // input wire [0 : 0] web
  .addrb(sprite_ram_addrb),  // input wire [9 : 0] addrb
  .dinb(sprite_ram_dinb),    // input wire [31 : 0] dinb
  .doutb(sprite_ram_doutb)  // output wire [31 : 0] doutb
);

logic tilemapram_ena, tilemapram_wea;
logic [10:0] tilemapram_addra;
logic [15:0] tilemapram_dina, tilemapram_douta;

assign tilemapram_ena = init_tilemapram_we;    // dummy
assign tilemapram_wea = init_tilemapram_we;    // dummy
assign tilemapram_addra = init_tilemapram_addr;  // dummy
assign tilemapram_dina = init_tilemapram_data;   // dummy
assign tilemapram_douta = 0;  // dummy
assign tilemapram_dinb = 0;

//tilemap_ram_bank00 tilemapram00 (
bram_tdp_rf_rf #(
  .ADDR_W(11),
  .DATA_W(16)
) tilemapram00 (
  .clka(clk10MHZ),    // input wire clka
  .ena(tilemapram_ena),
  .wea(tilemapram_wea),      // input wire [0 : 0] wea
  .addra(tilemapram_addra),  // input wire [10 : 0] addra
  .dina(tilemapram_dina),    // input wire [15 : 0] dina
  .douta(tilemapram_douta),  // output wire [15 : 0] douta
  .clkb(clk10MHZ),    // input wire clkb
  .enb(tilemapram_enb),
  .web(tilemapram_web),      // input wire [0 : 0] web
  .addrb(tilemapram_addrb),  // input wire [10 : 0] addrb
  .dinb(tilemapram_dinb),    // input wire [15 : 0] dinb
  .doutb(tilemapram_doutb)  // output wire [15 : 0] doutb
);

logic tileram_ena, tileram_wea;
logic [14:0] tileram_addra;
logic [ 7:0] tileram_dina, tileram_douta;

assign tileram_ena = init_tileram_we;    // dummy
assign tileram_wea = init_tileram_we;    // dummy
assign tileram_addra = init_tileram_addr;  // dummy
assign tileram_dina = init_tileram_data;   // dummy
assign tileram_douta = 0;  // dummy
assign tileram_dinb = 0;

//tile_ram_bank00 tileram00 (
bram_tdp_rf_rf #(
  .ADDR_W(15),
  .DATA_W(8)
) tileram00 (
  .clka(clk10MHZ),    // input wire clka
  .ena(tileram_ena),
  .wea(tileram_wea),      // input wire [0 : 0] wea
  .addra(tileram_addra),  // input wire [14 : 0] addra
  .dina(tileram_dina),    // input wire [7 : 0] dina
  .douta(tileram_douta),  // output wire [7 : 0] douta
  .clkb(clk10MHZ),    // input wire clkb
  .enb(tileram_enb),
  .web(tileram_web),      // input wire [0 : 0] web
  .addrb(tileram_addrb),  // input wire [14 : 0] addrb
  .dinb(tileram_dinb),    // input wire [7 : 0] dinb
  .doutb(tileram_doutb)  // output wire [7 : 0] doutb
);

logic palram_ena, palram_wea;
logic [7:0] palram_addra;
logic [31:0] palram_dina, palram_douta;

assign palram_ena = init_palram_we;    // dummy
assign palram_wea = init_palram_we;    // dummy
assign palram_addra = init_palram_addr;  // dummy
assign palram_dina = init_palram_data;   // dummy
assign palram_douta = 0;  // dummy
assign palram_dinb = 0;

//palette_ram palram00 (
bram_tdp_rf_rf #(
  .ADDR_W(8),
  .DATA_W(32)
) palram00 (
  .clka(clk10MHZ),    // input wire clka
  .ena(palram_ena),
  .wea(palram_wea),      // input wire [0 : 0] wea
  .addra(palram_addra),  // input wire [7 : 0] addra
  .dina(palram_dina),    // input wire [31 : 0] dina
  .douta(palram_douta),  // output wire [31 : 0] douta
  .clkb(clk10MHZ),    // input wire clkb
  .enb(palram_enb),
  .web(palram_web),      // input wire [0 : 0] web
  .addrb(palram_addrb),  // input wire [7 : 0] addrb
  .dinb(palram_dinb),    // input wire [31 : 0] dinb
  .doutb(palram_doutb)  // output wire [31 : 0] doutb
);


// [DEBUG]
vpu_core vpu_core(
  clk10MHZ,
  //clkPixel,  // DEBUG
  initialized,

  sprite_ram_enb,
  sprite_ram_addrb,
  sprite_ram_doutb,

  tilemapram_enb,
  tilemapram_addrb,
  tilemapram_doutb,

  tileram_enb,
  tileram_addrb,
  tileram_doutb,

  palram_enb,
  palram_addrb,
  palram_doutb,

  dot_clk,
  color,
  hsync_vpu,
  vsync_vpu
);

assign lcd_write = hsync_vpu && vsync_vpu;

ili9341_parallel_8bit tft0(
    clk10MHZ,  // 4clk per pixel
    reset,
    lcd_vblank,
    lcd_write,
    lcd_col_r,
    lcd_col_g,
    lcd_col_b,
    ck_a4,
    ck_a3,
    ck_a2,
    ck_a1,
    ck_a0,
    data8,
    initialized,
    hsync_out,
    vsync_out
);

endmodule
