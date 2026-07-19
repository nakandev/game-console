`default_nettype none

module arty_a7_100t_top
  import gameconsole_pkg::*;
(
  input wire CLK100MHZ,
  input wire ck_rst,

  output wire [13:0]ddr3_sdram_addr,
  output wire [2:0]ddr3_sdram_ba,
  output wire ddr3_sdram_cas_n,
  output wire [0:0]ddr3_sdram_ck_n,
  output wire [0:0]ddr3_sdram_ck_p,
  output wire [0:0]ddr3_sdram_cke,
  output wire [0:0]ddr3_sdram_cs_n,
  output wire [1:0]ddr3_sdram_dm,
  inout wire [15:0]ddr3_sdram_dq,
  inout wire [1:0]ddr3_sdram_dqs_n,
  inout wire [1:0]ddr3_sdram_dqs_p,
  output wire [0:0]ddr3_sdram_odt,
  output wire ddr3_sdram_ras_n,
  output wire ddr3_sdram_reset_n,
  output wire ddr3_sdram_we_n,

  input wire [3:0]btn_tri_i,
  input wire [3:0]sw,
  output wire [3:0]led_tri_o,

  input wire usb_uart_rxd,
  output wire usb_uart_txd,

  // output wire vga_hsync,      // Driven out via Pmod
  // output wire vga_vsync,      // Driven out via Pmod
  // output wire [3:0] vga_r,     // 4-bit Red
  // output wire [3:0] vga_g,     // 4-bit Green
  // output wire [3:0] vga_b,     // 4-bit Blue

  inout wire Pmod_sd_out_0_pin1_io,
  inout wire Pmod_sd_out_0_pin2_io,
  inout wire Pmod_sd_out_0_pin3_io,
  inout wire Pmod_sd_out_0_pin4_io,
  inout wire Pmod_sd_out_0_pin7_io,
  inout wire Pmod_sd_out_0_pin8_io,
  inout wire Pmod_sd_out_0_pin9_io,
  inout wire Pmod_sd_out_0_pin10_io,

  output wire ck_a5,  // none
  output wire ck_a4,  // LCD_RST
  output wire ck_a3,  // LCD_CS
  output wire ck_a2,  // LCD_RS
  output wire ck_a1,  // LCD_WR
  output wire ck_a0,  // LCD_RD
  output wire ck_io8,  // LCD_D0
  output wire ck_io9,  // LCD_D1
  output wire ck_io2,  // LCD_D2
  output wire ck_io3,  // LCD_D3
  output wire ck_io4,  // LCD_D4
  output wire ck_io5,  // LCD_D5
  output wire ck_io6,  // LCD_D6
  output wire ck_io7   // LCD_D7
);

wire clk_cpu;
wire clk_vga;

wire [11:0]BRAM_PORTA_0_addr;
wire BRAM_PORTA_0_clk;
wire [31:0]BRAM_PORTA_0_din;
wire [31:0]BRAM_PORTA_0_dout;
wire BRAM_PORTA_0_en;
wire BRAM_PORTA_0_rst;
wire [3:0]BRAM_PORTA_0_we;
wire [12:0]BRAM_PORTA_1_addr;
wire BRAM_PORTA_1_clk;
wire [31:0]BRAM_PORTA_1_din;
wire [31:0]BRAM_PORTA_1_dout;
wire BRAM_PORTA_1_en;
wire BRAM_PORTA_1_rst;
wire [3:0]BRAM_PORTA_1_we;
wire [16:0]BRAM_PORTA_2_addr;
wire BRAM_PORTA_2_clk;
wire [31:0]BRAM_PORTA_2_din;
wire [31:0]BRAM_PORTA_2_dout;
wire BRAM_PORTA_2_en;
wire BRAM_PORTA_2_rst;
wire [3:0]BRAM_PORTA_2_we;
wire [11:0]BRAM_PORTA_3_addr;
wire BRAM_PORTA_3_clk;
wire [31:0]BRAM_PORTA_3_din;
wire [31:0]BRAM_PORTA_3_dout;
wire BRAM_PORTA_3_en;
wire BRAM_PORTA_3_rst;
wire [3:0]BRAM_PORTA_3_we;

reg [31:0] vram_info_tri_io;

system_wrapper system_wrapper_0
(
    BRAM_PORTA_0_addr,
    BRAM_PORTA_0_clk,
    BRAM_PORTA_0_din,
    BRAM_PORTA_0_dout,
    BRAM_PORTA_0_en,
    BRAM_PORTA_0_rst,
    BRAM_PORTA_0_we,
    BRAM_PORTA_1_addr,
    BRAM_PORTA_1_clk,
    BRAM_PORTA_1_din,
    BRAM_PORTA_1_dout,
    BRAM_PORTA_1_en,
    BRAM_PORTA_1_rst,
    BRAM_PORTA_1_we,
    BRAM_PORTA_2_addr,
    BRAM_PORTA_2_clk,
    BRAM_PORTA_2_din,
    BRAM_PORTA_2_dout,
    BRAM_PORTA_2_en,
    BRAM_PORTA_2_rst,
    BRAM_PORTA_2_we,
    BRAM_PORTA_3_addr,
    BRAM_PORTA_3_clk,
    BRAM_PORTA_3_din,
    BRAM_PORTA_3_dout,
    BRAM_PORTA_3_en,
    BRAM_PORTA_3_rst,
    BRAM_PORTA_3_we,

    CLK100MHZ,
    Pmod_sd_out_0_pin10_io,
    Pmod_sd_out_0_pin1_io,
    Pmod_sd_out_0_pin2_io,
    Pmod_sd_out_0_pin3_io,
    Pmod_sd_out_0_pin4_io,
    Pmod_sd_out_0_pin7_io,
    Pmod_sd_out_0_pin8_io,
    Pmod_sd_out_0_pin9_io,
    btn_tri_i,
    ck_rst,
    clk_cpu,
    clk_vga,
    ddr3_sdram_addr,
    ddr3_sdram_ba,
    ddr3_sdram_cas_n,
    ddr3_sdram_ck_n,
    ddr3_sdram_ck_p,
    ddr3_sdram_cke,
    ddr3_sdram_cs_n,
    ddr3_sdram_dm,
    ddr3_sdram_dq,
    ddr3_sdram_dqs_n,
    ddr3_sdram_dqs_p,
    ddr3_sdram_odt,
    ddr3_sdram_ras_n,
    ddr3_sdram_reset_n,
    ddr3_sdram_we_n,
    led_tri_o,
    usb_uart_rxd,
    usb_uart_txd,
    vram_info_tri_io
);



wire initialized;
logic lcd_vblank;
logic lcd_write;
logic lcd_reset;

logic [7:0] lcd_col_r;
logic [7:0] lcd_col_g;
logic [7:0] lcd_col_b;
wire [7:0] data8;
logic hdraw_out;
logic vdraw_out;

logic dot_clk;
logic [31:0] color;
logic hdraw_vpu;
logic vdraw_vpu;

// localparam VH_DELAY = 16;
// logic [0:0] vdelay[VH_DELAY];
// logic [0:0] hdelay[VH_DELAY];
// always_ff @(posedge clk_cpu_0) begin
//   hdelay[0] <= hdraw_vpu;
//   vdelay[0] <= vdraw_vpu;
//   for (int i=1; i<VH_DELAY; i++) begin
//     hdelay[i] <= hdelay[i-1];
//     vdelay[i] <= vdelay[i-1];
//   end
// assign lcd_reset = ~ck_rst;
// assign lcd_reset = btn_tri_i[0];
assign lcd_reset = sw[0];
assign lcd_write = hdraw_vpu && vdraw_vpu;

vpu vpu(
  .clk     (clk_cpu),
  .rst_n   (initialized),

  .cpu_param_ram_en  (BRAM_PORTA_0_en  ),
  .cpu_param_ram_we  (BRAM_PORTA_0_we  ),
  .cpu_param_ram_addr(BRAM_PORTA_0_addr[11:2]),
  .cpu_param_ram_din (BRAM_PORTA_0_din ),
  .cpu_param_ram_dout(BRAM_PORTA_0_dout),

  .cpu_map_ram_en    (BRAM_PORTA_1_en  ),
  .cpu_map_ram_we    (BRAM_PORTA_1_we  ),
  .cpu_map_ram_addr  (BRAM_PORTA_1_addr[12:2]),
  .cpu_map_ram_din   (BRAM_PORTA_1_din ),
  .cpu_map_ram_dout  (BRAM_PORTA_1_dout),

  .cpu_tile_ram_en   (BRAM_PORTA_2_en  ),
  .cpu_tile_ram_we   (BRAM_PORTA_2_we  ),
  .cpu_tile_ram_addr (BRAM_PORTA_2_addr[16:2]),
  .cpu_tile_ram_din  (BRAM_PORTA_2_din ),
  .cpu_tile_ram_dout (BRAM_PORTA_2_dout),

  .cpu_pal_ram_en    (BRAM_PORTA_3_en  ),
  .cpu_pal_ram_we    (BRAM_PORTA_3_we  ),
  .cpu_pal_ram_addr  (BRAM_PORTA_3_addr[10:2]),
  .cpu_pal_ram_din   (BRAM_PORTA_3_din ),
  .cpu_pal_ram_dout  (BRAM_PORTA_3_dout),

  .dot_clk (dot_clk    ),
  .color   (color      ),
  .hdraw   (hdraw_vpu  ),
  .vdraw   (vdraw_vpu  )
);

assign vram_info_tri_io[2] = initialized;
assign vram_info_tri_io[1] = hdraw_vpu;
assign vram_info_tri_io[0] = vdraw_vpu;

logic clkPixel = 1'b0; // 4 clk_cpu
logic [31:0] count_for_clkpixel = 1;
always_ff @(posedge clk_cpu) begin
    if (count_for_clkpixel > 0) begin
        count_for_clkpixel <= count_for_clkpixel - 1'b1;
    end else begin
        count_for_clkpixel <= 1;
        clkPixel <= ~clkPixel;
    end
end

always_ff @(posedge clkPixel) begin
    //lcd_col_a <= color[31:24];
    lcd_col_b <= color[23:16];
    lcd_col_g <= color[15: 8];
    lcd_col_r <= color[ 7: 0];
end

ili9341_parallel_8bit tft0(
    clk_cpu,  // 4clk per pixel
    lcd_reset,
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
    hdraw_out,
    vdraw_out
);

assign {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8} = data8;

// wire [9:0] vga_x;
// wire [9:0] vga_y;
// wire video_on;

// // Instantiate your sync controller
// vga_controller vga_inst (
//     // .clk_25MHz(clk_vga_0),
//     .clk_100MHz(CLK100MHZ),
//     .reset(btn[0]),
//     .hsync(vga_hsync),
//     .vsync(vga_vsync),
//     .pixel_x(vga_x),
//     .pixel_y(vga_y),
//     .video_on(video_on)
// );

// assign vga_r = video_on ? lcd_col_r : 4'h0;
// assign vga_g = video_on ? lcd_col_g : 4'h0;
// assign vga_b = video_on ? lcd_col_b : 4'h0;

// assign led[0] = btn[0];
// assign led[1] = clk_cpu;
// assign led[2] = vdraw_out;
// assign led[3] = initialized;

endmodule
