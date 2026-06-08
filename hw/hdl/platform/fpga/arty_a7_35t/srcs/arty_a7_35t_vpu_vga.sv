`default_nettype none

module arty_a7_35t_vpu_ili9341_parallel_8bit
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
    output logic [3:0] led
);
  
localparam DOTCLKS = 2; // 1:50MHz, 2:25MHz.
localparam PIXCLKS = 2; // 2cycle

logic clk10MHZ = 1'b0;
logic clkPixel = 1'b0; // 4 clk10MHZ
// logic clkTFT = 1'b0;
logic [31:0] cnt0 = DOTCLKS-1;
logic [31:0] cnt1 = PIXCLKS-1;
wire [7:0] data8;
logic [7:0] lcd_col_r;
logic [7:0] lcd_col_g;
logic [7:0] lcd_col_b;
logic reset200;
logic reset;
wire initialized;
logic hdraw_out;
logic vdraw_out;
logic prev_initialized;
logic lcd_vblank;
logic lcd_write;
logic [3:0] dummy_led;

assign reset200 = btn[0];
assign reset = btn[1];
// assign data8 = {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8};
assign {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8} = data8;

logic CLK200MHZ;
logic locked;

reg        mem_en;
reg        mem_we;
reg [31:0] mem_addr;
reg [31:0] mem_din;
reg [31:0] mem_dout;

logic dot_clk;
logic [31:0] color;
logic hdraw_vpu;
logic vdraw_vpu;

assign led = {1'b0, hdraw_vpu, vdraw_vpu, initialized};
  
initial begin
      // initialized = 1'b0;
      // hdraw_out = 1'b0;
      // vdraw_out = 1'b0;
      // prev_initialized = 0;
      // prev_hdraw_out = 1'b0;
      // prev_vdraw_out = 1'b0;
      // lcd_vblank = 1'b0;
      // lcd_write = 1'b1;
      // dummy_led = 0;
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
always @(posedge CLK100MHZ) begin
    if (cnt0 > 0) begin
        cnt0 <= cnt0 - 1'b1;
    end else begin
        cnt0 <= DOTCLKS-1;
        clk10MHZ <= ~clk10MHZ;
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

cpu cpu(
  .clk     (clk10MHZ),
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
  .clk     (clk10MHZ   ),
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
always_ff @(posedge clk10MHZ) begin
  hdelay[0] <= hdraw_vpu;
  vdelay[0] <= vdraw_vpu;
  for (int i=1; i<VH_DELAY; i++) begin
    hdelay[i] <= hdelay[i-1];
    vdelay[i] <= vdelay[i-1];
  end
end

assign lcd_write = hdraw_vpu && vdraw_vpu;
// assign lcd_write = hdelay[VH_DELAY-1] && vdelay[VH_DELAY-1];
// assign lcd_write = hdelay[16-1] && vdelay[16-1];

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
    hdraw_out,
    vdraw_out
);

logic [7:0] count = 0;

logic [15:0] color16;
reg [31:0] color_buf_in;
reg [31:0] color_buf_out;
reg [9:0] buf_x = 0;
reg [9:0] buf_y = 0;

assign color16 = {1'd0, color[24:20], color[15:11], color[7:3]};

always_ff @(posedge clkPixel) begin
    //lcd_col_a <= color[31:24];
    // lcd_col_b <= color[23:16];
    // lcd_col_g <= color[15: 8];
    lcd_col_g <= color[23:16];
    lcd_col_b <= color[15: 8];
    lcd_col_r <= color[ 7: 0];
    color_buf_in[31:0] <= color[31:0];
    count <= count + 1;
end

always_ff @(posedge clkPixel) begin
  if (!hdraw_vpu) begin
    buf_x <= 0;
  end
  else
  begin
    if (buf_x < (SCREEN_W + SCREEN_HBLANK)-1) begin
      buf_x <= buf_x + 1;
    end else begin
      buf_x <= 0;
    end
  end
end

always_ff @(posedge clkPixel) begin
  if (!vdraw_vpu) begin
    buf_y <= 0;
  end
  else
  begin
    if (buf_y < (SCREEN_H + SCREEN_VBLANK)-1) begin
      buf_y <= buf_y + 1;
    end else begin
      buf_y <= 0;
    end
  end
end

wire [9:0] vga_x;
wire [9:0] vga_y;
wire video_on;

// Instantiate your sync controller
vga_controller vga_inst (
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
        vga_b <= color_buf_out[23:16];
        vga_g <= color_buf_out[15: 8];
        vga_r <= color_buf_out[ 7: 0];
    end
end


bram_tdp_rf_rf #(
  // .ADDR_W(15),
  // .DATA_W(16)
  .ADDR_W(10),
  .DATA_W(32)
) colorbuf_ram (
  .clka (clkPixel),
  .ena  (hdraw_vpu),
  .wea  (1),
  .addra(buf_x),
  .dina (color_buf_in),
  .douta(0),
  .clkb (CLK100MHZ),
  .enb  (1),
  .web  (0),
  .addrb({1'd0, vga_x[9:1]}),
  .dinb (0),
  .doutb(color_buf_out)
);

endmodule
