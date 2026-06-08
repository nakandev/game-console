`default_nettype none

module arty_a7_35t_top
  import gameconsole_pkg::*;
(
  input wire CLK100MHZ,
  input wire [3:0] btn,

  output wire vga_hsync,      // Driven out via Pmod
  output wire vga_vsync,      // Driven out via Pmod
  output reg [3:0] vga_r,     // 4-bit Red
  output reg [3:0] vga_g,     // 4-bit Green
  output reg [3:0] vga_b,     // 4-bit Blue

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

  output wire [13:0]ddr3_sdram_addr,
  output wire [2:0]ddr3_sdram_ba,
  output wire ddr3_sdram_cas_n,
  output wire [0:0]ddr3_sdram_ck_n,
  output wire [0:0]ddr3_sdram_ck_p,
  output wire [0:0]ddr3_sdram_cke,
  output wire [0:0]ddr3_sdram_cs_n,
  output wire [1:0]ddr3_sdram_dm,
  inout  wire [15:0]ddr3_sdram_dq,
  inout  wire [1:0]ddr3_sdram_dqs_n,
  inout  wire [1:0]ddr3_sdram_dqs_p,
  output wire [0:0]ddr3_sdram_odt,
  output wire ddr3_sdram_ras_n,
  output wire ddr3_sdram_reset_n,
  output wire ddr3_sdram_we_n,

  output logic [3:0] led
);

logic clk_100mhz_0;
logic clk_cpu_0;
logic clk_vga_0;

design_1_wrapper design_1_wrapper_inst(
  .clk_100mhz_0          (clk_100mhz_0),  // output
  .clk_cpu_0             (clk_cpu_0),  // output
  .clk_vga_0             (clk_vga_0),  // output
  .ddr3_sdram_addr       (ddr3_sdram_addr   ),  // output
  .ddr3_sdram_ba         (ddr3_sdram_ba     ),  // output
  .ddr3_sdram_cas_n      (ddr3_sdram_cas_n  ),  // output
  .ddr3_sdram_ck_n       (ddr3_sdram_ck_n   ),  // output
  .ddr3_sdram_ck_p       (ddr3_sdram_ck_p   ),  // output
  .ddr3_sdram_cke        (ddr3_sdram_cke    ),  // output
  .ddr3_sdram_cs_n       (ddr3_sdram_cs_n   ),  // output
  .ddr3_sdram_dm         (ddr3_sdram_dm     ),  // output
  .ddr3_sdram_dq         (ddr3_sdram_dq     ),  // inout
  .ddr3_sdram_dqs_n      (ddr3_sdram_dqs_n  ),  // inout
  .ddr3_sdram_dqs_p      (ddr3_sdram_dqs_p  ),  // inout
  .ddr3_sdram_odt        (ddr3_sdram_odt    ),  // output
  .ddr3_sdram_ras_n      (ddr3_sdram_ras_n  ),  // output
  .ddr3_sdram_reset_n    (ddr3_sdram_reset_n),  // output
  .ddr3_sdram_we_n       (ddr3_sdram_we_n   ),  // output
  .reset                 (btn[0]),  // input
  .sys_clk_i             (CLK100MHZ),  // input
  .usb_uart_rxd          (),  // input
  .usb_uart_txd          ()   // output
);


reg        mem_en;
reg        mem_we;
reg [31:0] mem_addr;
reg [31:0] mem_din;
reg [31:0] mem_dout;


wire initialized;
logic lcd_vblank;
logic lcd_write;
logic lcd_reset;
assign lcd_reset = btn[1];
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

cpu cpu(
  .clk     (clk_cpu_0),
  .rst_n   (initialized),
  // .rst_n   (~reset),
  .vdraw   (vdraw_vpu),  // [DEBUG]
  .mem_en  (mem_en  ),
  .mem_we  (mem_we  ),
  .mem_addr(mem_addr),
  .mem_din (mem_din ),
  .mem_dout(mem_dout)
);

vpu vpu(
  .clk     (clk_cpu_0),
  .rst_n   (initialized),
  // .rst_n   (~reset),
  .mem_en  (mem_en     ),
  .mem_we  (mem_we     ),
  .mem_addr(mem_addr   ),
  .mem_din (mem_din    ),
  .mem_dout(mem_dout   ),
  .dot_clk (dot_clk    ),
  .color   (color      ),
  .hdraw   (hdraw_vpu  ),
  .vdraw   (vdraw_vpu  )
);

localparam VH_DELAY = 16;
logic [0:0] vdelay[VH_DELAY];
logic [0:0] hdelay[VH_DELAY];
always_ff @(posedge clk_cpu_0) begin
  hdelay[0] <= hdraw_vpu;
  vdelay[0] <= vdraw_vpu;
  for (int i=1; i<VH_DELAY; i++) begin
    hdelay[i] <= hdelay[i-1];
    vdelay[i] <= vdelay[i-1];
  end
end

assign lcd_write = hdraw_vpu && vdraw_vpu;

ili9341_parallel_8bit tft0(
    clk_cpu_0,  // 4clk per pixel
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

localparam PIXCLKS = 2; // 2cycle
logic clkPixel = 1'b0; // 4 clk10MHZ
logic [31:0] cnt1 = PIXCLKS-1;

always_ff @(posedge clk_cpu_0) begin
    if (cnt1 > 0) begin
        cnt1 <= cnt1 - 1'b1;
    end else begin
        cnt1 <= PIXCLKS-1;
        clkPixel <= ~clkPixel;
    end
end

always_ff @(posedge clkPixel) begin
    lcd_col_r <= color[ 7: 0];
    lcd_col_g <= color[15: 8];
    lcd_col_b <= color[23:16];
    // lcd_col_b <= color[15: 8];
    // lcd_col_g <= color[23:16];
    //lcd_col_a <= color[31:24];
    // color_buf_in[31:0] <= color[31:0];

    // lcr_col_r <= 8'hff;
    // lcd_col_g <= 8'h00;
    // lcd_col_b <= 8'hff;
end

wire [9:0] vga_x;
wire [9:0] vga_y;
wire video_on;

// Instantiate your sync controller
vga_controller vga_inst (
    // .clk_25MHz(clk_vga_0),
    .clk_100MHz(CLK100MHZ),
    .reset(btn[0]),
    .hsync(vga_hsync),
    .vsync(vga_vsync),
    .pixel_x(vga_x),
    .pixel_y(vga_y),
    .video_on(video_on)
);

always @(*) begin
    if (!video_on) begin
        vga_r = 4'h0;
        vga_g = 4'h0;
        vga_b = 4'h0;
    end else begin
        //vga_b <= color_buf_out[23:16];
        //vga_g <= color_buf_out[15: 8];
        //vga_r <= color_buf_out[ 7: 0];
        vga_r <= lcd_col_r;
        vga_g <= lcd_col_g;
        vga_b <= lcd_col_b;
    end
end

assign led[0] = btn[0];
assign led[1] = clk_cpu_0;
assign led[2] = vdraw_out;
assign led[3] = initialized;

endmodule
