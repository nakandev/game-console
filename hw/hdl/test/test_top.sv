`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 03/12/2024 04:36:01 PM
// Design Name: 
// Module Name: test_top
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

//`include "top.sv"

module test_arty_a7_35t_vpu_ili9341_parallel_8bit(
);

  logic CLK100MHZ;
  logic [3:0] btn;

  logic ck_a5;
  logic ck_a4;
  logic ck_a3;
  logic ck_a2;
  logic ck_a1;
  logic ck_a0;
  logic ck_io8;
  logic ck_io9;
  logic ck_io2;
  logic ck_io3;
  logic ck_io4;
  logic ck_io5;
  logic ck_io6;
  logic ck_io7;
  logic [3:0] led;

  initial begin
    CLK100MHZ = 0;
    btn = 0;

    ck_a5   = 0;
    ck_a4   = 0;
    ck_a3   = 0;
    ck_a2   = 0;
    ck_a1   = 0;
    ck_a0   = 0;
    ck_io8  = 0;
    ck_io9  = 0;
    ck_io2  = 0;
    ck_io3  = 0;
    ck_io4  = 0;
    ck_io5  = 0;
    ck_io6  = 0;
    ck_io7  = 0;
    led     = 0;
  end

  arty_a7_35t_vpu_ili9341_parallel_8bit top_Ins (
    CLK100MHZ,
    btn,
    ck_a5,  // none
    ck_a4,  // LCD_RST
    ck_a3,  // LCD_CS
    ck_a2,  // LCD_RS
    ck_a1,  // LCD_WR
    ck_a0,  // LCD_RD
    ck_io8,  // LCD_D0
    ck_io9,  // LCD_D1
    ck_io2,  // LCD_D2
    ck_io3,  // LCD_D3
    ck_io4,  // LCD_D4
    ck_io5,  // LCD_D5
    ck_io6,  // LCD_D6
    ck_io7,  // LCD_D7
    led
  );

  always begin
    #5; CLK100MHZ <= ~CLK100MHZ;
  end

  initial begin
    btn[0] <= 1'b0;
    #40; btn[0] <= 1'b1;
    #40; btn[0] <= 1'b0;
    #1000; btn[1] <= 1'b1;
    #40;   btn[1] <= 1'b0;
    //#25000000
    #80000000;
    #1000; btn[1] <= 1'b1;
    #40;   btn[1] <= 1'b0;
    #20000000;
    $finish;
  end

endmodule

