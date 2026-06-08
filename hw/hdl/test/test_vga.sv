module top_vga (
  input wire CLK100MHZ,       // 100 MHz clock from Arty Pin E3
  input wire[3:0] btn,            // Reset signal mapped to Push Button 0
  output wire vga_hsync,      // Driven out via Pmod
  output wire vga_vsync,      // Driven out via Pmod
  output reg [3:0] vga_r,     // 4-bit Red
  output reg [3:0] vga_g,     // 4-bit Green
  output reg [3:0] vga_b,     // 4-bit Blue

  logic ck_a5,
  logic ck_a4,
  logic ck_a3,
  logic ck_a2,
  logic ck_a1,
  logic ck_a0,
  logic ck_io8,
  logic ck_io9,
  logic ck_io2,
  logic ck_io3,
  logic ck_io4,
  logic ck_io5,
  logic ck_io6,
  logic ck_io7,
  logic [3:0] led
);

  assign ck_a5  = 1'd0;
  assign ck_a4  = 1'd0;
  assign ck_a3  = 1'd0;
  assign ck_a2  = 1'd0;
  assign ck_a1  = 1'd0;
  assign ck_a0  = 1'd0;
  assign ck_io8 = 1'd0;
  assign ck_io9 = 1'd0;
  assign ck_io2 = 1'd0;
  assign ck_io3 = 1'd0;
  assign ck_io4 = 1'd0;
  assign ck_io5 = 1'd0;
  assign ck_io6 = 1'd0;
  assign ck_io7 = 1'd0;
  assign led[3:1] = 3'd0;

    wire [9:0] x;
    wire [9:0] y;
    wire video_on;

    // Instantiate your sync controller
    vga_controller vga_inst (
        .clk_100MHz(CLK100MHZ),
        .reset(btn[0]),
        .hsync(vga_hsync),
        .vsync(vga_vsync),
        .pixel_x(x),
        .pixel_y(y),
        .video_on(video_on)
    );

    // Color generation (Displaying structural colored columns)
    always @(*) begin
        if (!video_on) begin
            vga_r = 4'b0000;
            vga_g = 4'b0000;
            vga_b = 4'b0000;
        end else begin
            if (y < 20 && x == y) begin
                vga_r = 4'hF;
                vga_g = 4'hF;
                vga_b = 4'hF;
            end
            else
            if (x < 213) begin          // First column: Red
                vga_r = 4'hF;
                vga_g = 4'h0;
                vga_b = 4'h0;
            end else if (x < 426) begin   // Second column: Green
                vga_r = 4'h0;
                vga_g = 4'hF;
                vga_b = 4'h0;
            end else begin              // Third column: Blue
                vga_r = 4'h0;
                vga_g = 4'h0;
                vga_b = 4'hF;
            end
        end
    end
endmodule
