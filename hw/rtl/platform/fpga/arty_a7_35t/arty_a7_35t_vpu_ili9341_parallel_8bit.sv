`default_nettype none
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/22/2024 11:02:03 PM
// Design Name: 
// Module Name: top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

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
  
  localparam DOTCLKS = 4; //10/2;
  localparam PIXCLKS = 2; //2cycle

  logic clk10MHZ = 1'b0;
  logic clkPixel = 1'b0;
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
  
  initial begin
      initialized <= 1'b0;
//      hsync_out <= 1'b0;
//      vsync_out <= 1'b0;
//      prev_initialized <= 0;
//      prev_hsync_out <= 1'b0;
//      prev_vsync_out <= 1'b0;
//      lcd_vblank <= 1'b0;
      lcd_write <= 1'b1;
//      lcd_x <= 0;
//      lcd_y <= 0;
//      lcd_frame <= 0;
//      dummy_led <= 0;
  end

  clk_wiz_0 clk_100MHZ_to_200MHZ
   (
    // Clock out ports
    .clk_out1(CLK200MHZ),     // output clk_out1
    // Status and control signals
    .reset(reset200), // input reset
    .locked(locked),       // output locked
   // Clock in ports
    .clk_in1(CLK100MHZ)      // input clk_in1
  );

  always_ff @(posedge CLK200MHZ) begin
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
  end
  
  logic dot_clk;
  logic [31:0] color;
  logic hsync_vpu;
  logic vsync_vpu;
  assign lcd_col_r = color[31:24];
  assign lcd_col_g = color[23:16];
  assign lcd_col_b = color[15: 8];
  vpu vpu0(
    clkPixel,
    initialized, //!reset
    dot_clk,
    color,
    hsync_vpu,
    vsync_vpu
  );

  ili9341_parallel_8bit tft0(
    clk10MHZ,
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
